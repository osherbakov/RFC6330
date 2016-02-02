#include "RFC6330func.h"

#define swap(a,b) {unsigned char tmp; tmp = (a); (a) = (b); (b) = (tmp);}while(0)

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

int gf_gauss(unsigned char *Result, 
					  params_t * Params, 
					  unsigned char *A, 
					  unsigned char *Symbols, unsigned int BytesPerSymbol,
					  unsigned int NumSymbols)
{
/***********************************************
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
    if (A(row,col) ~= 1)
        % Normalize first coefficient of the chosen row
        coeff    = rfc6330_findcoeff( A(row,col), 1 );
        A(row,:) = rfc6330_gfmult_elementwise(coeff*ones(1,COLS),A(row,:));
        % Perform the same action to the corresponding symbol
        %disp(['Normalizing! Initially symbol at ',int2str(row),' = ',int2str(Symbol(row))])
        %disp(['Normalizing coefficient = ',int2str(coeff)])
        if (coeff == 1)
        elseif (Symbol(row) == 1)
            Symbol(row) = coeff;
        else
            Symbol(row) = rfc6330_gfmult(coeff,Symbol(row));
        end
        %disp(['After the normalization symbol at ',int2str(row),' = ',int2str(Symbol(row))])
    end
    % Eliminate rows
    for irow = (row+1):ROWS
        if (A(irow,col) ~= 0)
            %disp(['Symbol at position ',int2str(irow),' is ',int2str(Symbol(irow))])
            if (A(irow,col) == 1)
                coeff = Symbol(row);
            elseif (Symbol(row) == 1)
                coeff = A(irow,col);
            else
                coeff = rfc6330_gfmult(A(irow,col),Symbol(row));
            end
            Symbol(irow) = bitxor(coeff,Symbol(irow));
            mult_row     = rfc6330_gfmult_elementwise(A(irow,col)*ones(1,COLS),A(row,:));
            A(irow,:)    = bitxor(A(irow,:),mult_row);
        end
    end
end
A = A(1:COLS,1:COLS);
% Back-substitution
%disp('B A C K s u b s t i t u t i o n!')
for row = COLS-1:-1:1
    for col = (row+1):COLS
        if (A(row,col) ~= 0)
            if (Symbol(col) == 0)
                coeff = 0;
            elseif (Symbol(col) == 1)
                coeff = A(row,col);
            elseif (A(row,col) == 1)
                coeff = Symbol(col);
            else
                coeff = rfc6330_gfmult(A(row,col),Symbol(col));
            end
            Symbol(row) = bitxor(Symbol(row),coeff);
            A(row,:) = bitxor(A(row,:),...
                rfc6330_gfmult_elementwise(A(row,col)*ones(1,COLS),A(col,:)));
            %disp(['Symbol at position ',int2str(col),' is ',int2str(Symbol(col))])
        end
    end
end
%rowexchanges
returnSymbols = Symbol(1:COLS);
************************************************/
    int ROWS, COLS, row, col, pivot_row;
	unsigned char coeff, *pData, *pPivot;

	ROWS = NumSymbols;
	COLS = Params->L;

	for(col = 0; col < COLS; col++)
	{
		row = col;
		// Find the first non-zero row
		pData = A + row * COLS + col;	// A[row, col]
		for(pivot_row = row; (*pData == 0) && (pivot_row < ROWS); pivot_row++, pData += COLS){}
		if((pivot_row < ROWS) && (pivot_row != row))				// If there is a pivot row - bring it to the top...
		{
			pPivot = A + pivot_row * COLS + col;  // A[pivot_row, col]
			pData = A + row * COLS + col;		// A[row, col]
			for (int i = col; i < COLS; i++, pData++, pPivot++)
			{
				swap(*pData, *pPivot);
			}
			vec_swap(&Symbols[row * BytesPerSymbol], &Symbols[pivot_row * BytesPerSymbol], BytesPerSymbol);
		}

		pData =  A + row * COLS + col;	// A[row, col]
		if(*pData == 0)
		{
			return -1;			// Error - Cannot be solved...
		}
		// Normalize first coeff on the diagonal
		if(*pData != 1)
		{
			coeff = gf_div(1, *pData);
			for (int i = col; i < COLS; i++, pData++)
			{
				*pData = gf_mult(*pData, coeff);
			}
			// Symbols[row] = rfc6330_gf_mult(Symbols[row],coeff);
			gf_vec_scale(&Symbols[row * BytesPerSymbol], &Symbols[row * BytesPerSymbol], coeff, BytesPerSymbol);
		}
		// Eliminate rows by adding to every row below scaled top one 	
		for (int irow = row + 1; irow < ROWS; irow++)
		{
			pPivot = A + row * COLS + col;		// A[row, col]
			pData = A + irow * COLS + col;		// A[irow, col]
			coeff = *pData;
			if(coeff)
			{
				for(int i = col; i < COLS; i++, pPivot++, pData++)
				{
					*pData ^= gf_mult(*pPivot, coeff);
				}
				gf_vec_scale_sub(&Symbols[irow * BytesPerSymbol], &Symbols[row * BytesPerSymbol], coeff, BytesPerSymbol);
			}
		}
	}

	// Do Backsubstitution
	for(row = COLS - 2; row >= 0; row--)
	{
		col = row + 1;
		pData = A + row * COLS + col;	// A[row,col]
		for (; col < COLS; col++, pData++)
		{
			coeff = *pData;
			if(coeff)
			{
				gf_vec_scale_add(&Symbols[row * BytesPerSymbol], &Symbols[col * BytesPerSymbol], coeff, BytesPerSymbol);
			}
		}
	}
	
	if(Result)
		vec_copy(Result, Symbols, BytesPerSymbol * COLS);
		
   return 0;
}
