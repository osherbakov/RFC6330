#include "rfc6330_func.h"

/***************************************
% Reduce matrix in row echelon form
for col = 1:COLS
    row = col;
    % search for a pivot
    ind = find(A(row:end,col)) + row - 1;
    if (isempty(ind))
        % If cannot find any non zero row decoding failed!
        disp('Decoding failed in Gaussian Elimination!')      % Always?
        returnSymbols = 0;
        return
    else
        pivot_row = ind(1);
    end

	% Exchange pivot row with the first row of matrix A
    % Exchange symbols
    if (pivot_row ~= row)
        temp           = A(row,:);
        A(row,:)       = A(pivot_row,:);
        A(pivot_row,:) = temp;
  
		TmpSymbol         = Symbol(col);
        Symbol(col)       = Symbol(pivot_row);
        Symbol(pivot_row) = TmpSymbol;
    end

	% Normalize first coefficient of the chosen row
    coeff    = rfc6330_findcoeff( A(row,col), 1 );
    A(row,:) = rfc6330_gfmult_elementwise(coeff*ones(1,COLS),A(row,:));
    % Perform the same action to the corresponding symbol
    Symbol(row) = rfc6330_gfmult(coeff,Symbol(row));

	% Eliminate rows
    for irow = (row+1):ROWS
		coeff = rfc6330_gfmult(A(irow,col),Symbol(row));
        Symbol(irow) = bitxor(coeff,Symbol(irow));
        mult_row     = rfc6330_gfmult_elementwise(A(irow,col)*ones(1,COLS),A(row,:));
        A(irow,:)    = bitxor(A(irow,:),mult_row);
    end
end
****************************************/

int rfc6330_gf_gauss(unsigned char *Result, 
					  unsigned char *A, 
					  unsigned char *Symbols, unsigned int Size)
{
   unsigned int row, i, j, pivot_row;
   unsigned char max, dum, *pa, *pA, *A_pivot_row;

      // for each variable find pivot row and perform forward substitution

   pa = A;
   for (row = 0; row < (Size - 1); row++, pa += Size) 
   {
     //  find the pivot row
      A_pivot_row = pa;
      max = *(pa + row);
      pA = pa + Size;
      pivot_row = row;
      for (i = row + 1; i < Size; pA += Size, i++)
         if ((dum = *(pA + row)) > max) 
		 { 
            max = dum; 
			A_pivot_row = pA; 
			pivot_row = i; 
         }
      if (max == 0) return -1;                // the matrix A is singular

      // and if it differs from the current row, interchange the two rows.           
      if (pivot_row != row) 
	  {
         for (i = row; i < Size; i++) 
		 {
            dum = *(pa + i);
            *(pa + i) = *(A_pivot_row + i);
            *(A_pivot_row + i) = dum;
         }
         dum = Symbols[row];
         Symbols[row] = Symbols[pivot_row];
         Symbols[pivot_row] = dum;
      }

      // Perform forward substitution
      for (i = row + 1; i < Size; i++) 
	  {
         pA = A + i * Size;
         dum =  rfc6330_gf_div(*(pA + row) , *(pa + row));
         *(pA + row) = 0;
         for (j = row + 1; j < Size; j++)
		 {
			*(pA + j) ^= rfc6330_gf_mult( dum , *(pa + j));
		 }
         Symbols[i] ^= rfc6330_gf_mult( dum , Symbols[row]);
      }
   }

   // Perform backward substitution
   pa = A + (Size - 1) * Size;
   for (row = Size - 1; row >= 0; pa -= Size, row--) 
   {
      if ( *(pa + row) == 0 )   return -1;   // matrix is singular
      dum = *(pa + row);
      for ( i = row + 1; i < Size; i++)
	  {
		  *(pa + i) = rfc6330_gf_div(*(pa + i), dum); 
	  }
	  Symbols[row] = rfc6330_gf_div(Symbols[row], dum); 

      for ( i = 0, pA = A; i < row; pA += Size, i++) 
	  {
         dum = *(pA + row);
         for ( j = row + 1; j < Size; j++)
		 {
			 *(pA + j) ^= rfc6330_gf_mult( dum,  *(pa + j));
		 }
		 Symbols[i] ^= rfc6330_gf_mult( dum , Symbols[row]);
      }
   }
   return 0;
}