/**************************************

function [IntermediateSymbols X] = rfc6330_inversion( A, Symbol, K )
%
% Implementation of the decoding algorithm described in the
% RFC6330 5.4. Example FEC Decoder
%
% I/O parameters
%                   A - matrix
%              Symbol - input symbols
% IntermediateSymbols - output symbols = inv(A)*Symbol

%In this specification, all the matrices have octets as entries, so it is understood
%that the operations of the matrix entries are to be done as stated in Section 5.7
%and A^^-1 is the matrix inverse of A with respect to octet arithmetic.
%
% Temporary parameters
% First Phase 
% ii - index i of the spec
% uu - index u of the spec
% V  - matrix to be transformed into identity
% Second Phase
% U  - U matrix of the spec
%
[ROWS COLS] = size(A);
%
% First Phase
% Initialize size of indices ii (i in the spec) and uu (u in the spec)
% Initialize matrix V
% Pick row with the correct weight r
% Declare decoding failure if a non zero row cannot be found
% Exchange rows if necessary
% Perform Gaussian Elimination to the subsequent rows if they start with a 1
% Increment ii = ii + 1, uu = uu + r - 1
%
% ii = 0;
% uu = 0;
%
% Initial indices of rows and columns - keep track of row/column exchanges
%
c = 1:COLS;                             % Intermediate Symbol mapping
d = (1:ROWS)';                          % source/received symbol mapping
%
% Matrix X - It should be lower triangular at the end of the first phase
%
X = A;
K_prime = rfc6330_K_prime( K );
[S H B U L W P] = rfc6330_parameters( K_prime );
ii = 0;
uu = P;

% [ii uu P L COLS]

hdpc = (S+1):(S+H);
process_hdpc = false;
processed = [zeros(1,S) ones(1,H) zeros(1,ROWS-S-H)];

while (ii + uu < COLS)
    % Choose matrix V to be tranformed into the identity matrix
    % Last step ii + uu = COLS - 1 -> ii + 1 = COLS - uu ->
    % matrix V is a column vector.
    V = A(ii+1:ROWS,ii+1:COLS-uu);
    row = ii + 1;
    [V_ROWS V_COLS] = size(V); %#ok<NASGU>
    weights = zeros(1,V_ROWS);
    for ind1 = 1:(V_ROWS)
        weights(ind1) = length(find(V(ind1,:)));
    end
    if (sum(weights) == 0)
        error(['Matrix V in phase 1 all zero! Step: ',int2str(row)])
    end
    min_weight = min(weights);
    r = 0;
    pivot_row = [];
    % While there are non HDPC rows to process select one of them
    if (process_hdpc == false)
        if (min_weight == 0)
            iter = 0;
            while (isempty(pivot_row))
                iter = iter + 1;
                if (iter > COLS)
                    pivot_row = find(weights) + row - 1;
                    if (isempty(pivot_row))
                        error('Decoding failure!')
                    end
                    pivot_row = pivot_row(1);
                    r = weights(pivot_row - row + 1);
                    break;
                end
                r = r + 1;
                pivot_row = find(weights == r) + row - 1;
                pivot_row = setdiff(pivot_row,hdpc);
            end
            min_weight = r;
        else
            r = min_weight - 1;
            iter = 0;
            while (isempty(pivot_row))
                iter = iter + 1;
                if (iter > COLS)
                    break;
                end
                r = r + 1;
                pivot_row = find(weights == r) + row - 1;
                pivot_row = setdiff(pivot_row,hdpc);
            end
            min_weight = r;
        end
        % If minimum weight is 2 pick one row from the maximum size
        % component
        if (min_weight == 2)
            rows2choose = rfc6330_find_max_size_component( A(pivot_row,ii+1:COLS-uu) );
            pivot_row = pivot_row(rows2choose);
        end
        % If pivot_row is a vector just make sure to pick the first element
        % of it. Update that this row has been processed.
        pivot_row = pivot_row(1);
        processed(d(pivot_row)) = 1;
        % Exchange chosen row with the first row in matrix V
        % Do the same exchange in matrix X
        % Update row exchange
        if (pivot_row ~= row)
            temp = A(row,:);
            A(row,:) = A(pivot_row,:);
            A(pivot_row,:) = temp;
            temp = X(row,:);
            X(row,:) = X(pivot_row,:);
            X(pivot_row,:) = temp;
            swap = d(row);
            d(row) = d(pivot_row);
            d(pivot_row) = swap;
        end
        % Bring first column with one at the beginning and all other columns
        % with one at the end
        col = row;
        indices = find(A(row,1:COLS-uu));
        nonzeroEl = length(indices);
        % First column with one at the beginning of matrix V
        if (indices(1) ~= col)
            temp = A(:,indices(1));
            A(:,indices(1)) = A(:,col);
            A(:,col) = temp;
            temp = X(:,indices(1));
            X(:,indices(1)) = X(:,col);
            X(:,col) = temp;
            swap = c(indices(1));
            c(indices(1)) = c(col);
            c(col) = swap;
        end
        % All the rest columns with a '1' in the selected row at the end
        if (nonzeroEl > 1)
            for jj = nonzeroEl:-1:2
                if (indices(jj) ~= COLS-uu+jj-nonzeroEl)
                    temp = A(:,indices(jj));
                    A(:,indices(jj)) = A(:,COLS-uu+jj-nonzeroEl);
                    A(:,COLS-uu+jj-nonzeroEl) = temp;
                    temp = X(:,indices(jj));
                    X(:,indices(jj)) = X(:,COLS-uu+jj-nonzeroEl);
                    X(:,COLS-uu+jj-nonzeroEl) = temp;
                    swap = c(indices(jj));
                    c(indices(jj)) = c(COLS-uu+jj-nonzeroEl);
                    c(COLS-uu+jj-nonzeroEl) = swap;
                end
            end
        end
        % Eliminate the subsequent non zero elements that appear in the
        % chosen column. Perform the appropriate operations to the
        % corresponding Symbol.
        for irow = row+1:ROWS
            if (A(irow,row) == 1)
                A(irow,:) = bitxor( A(irow,:), A(row,:) );
                Symbol(d(irow)) = bitxor( Symbol(d(irow)), Symbol(d(row)) );
            elseif (A(irow,row) ~= 0)
                % Multiply pivot_row with the multiplicative inverse of the
                % leading coefficient of it (A(row,col)) and then by the leading
                % coefficient of the current row (A(irow,col)). Then add
                % this row to the row A(irow,:)
                coeff = rfc6330_findcoeff(A(row,col),1);
                coeff = rfc6330_gfmult(coeff,A(irow,col));
                A(irow,:) = bitxor( A(irow,:),...
                    rfc6330_gfmult_elementwise(coeff*ones(1,COLS),A(row,:)) );
                Symbol(d(irow)) = bitxor( Symbol(d(irow)),...
                    rfc6330_gfmult(coeff,Symbol(d(row))) );
            end
        end
        for ind = 1:H
            hdpc(ind) = find(d == S + ind);
        end
        if (length(processed) == sum(processed))
            process_hdpc = true;
        end
    else
        if (min_weight == 0)
            while (isempty(pivot_row))
                r = r + 1;
                pivot_row = find(weights == r) + row - 1;
            end
            pivot_row = pivot_row(1);
            min_weight = r;
        else
            r = min_weight - 1;
            while (isempty(pivot_row))
                r = r + 1;
                pivot_row = find(weights == r) + row - 1;
            end
            pivot_row = pivot_row(1);
            min_weight = r;
        end
        % Exchange chosen row with the first row in matrix V
        % Update row exchange
        if (pivot_row ~= row)
            temp = A(row,:);
            A(row,:) = A(pivot_row,:);
            A(pivot_row,:) = temp;
            swap = d(row);
            d(row) = d(pivot_row);
            d(pivot_row) = swap;
        end
        % Bring first column with one at the beginning and all other columns
        % with one at the end
        col = row;
        indices = find(A(row,1:COLS-uu));
        nonzeroEl = length(indices);
        % First column with one at the beginning of matrix V
        if (indices(1) ~= col)
            temp = A(:,indices(1));
            A(:,indices(1)) = A(:,col);
            A(:,col) = temp;
            swap = c(indices(1));
            c(indices(1)) = c(col);
            c(col) = swap;
        end
        % All the rest columns with a '1' in the selected row at the end
        if (nonzeroEl > 1)
            for jj = nonzeroEl:-1:2
                if (indices(jj) ~= COLS-uu+jj-nonzeroEl)
                    temp = A(:,indices(jj));
                    A(:,indices(jj)) = A(:,COLS-uu+jj-nonzeroEl);
                    A(:,COLS-uu+jj-nonzeroEl) = temp;
                    swap = c(indices(jj));
                    c(indices(jj)) = c(COLS-uu+jj-nonzeroEl);
                    c(COLS-uu+jj-nonzeroEl) = swap;
                end
            end
        end
        X(row,:) = A(row,:);
        % xor chosen row with the subsequent rows that have a non zero value in their first position
        % xor respective D symbols.
        % D[d[i']] <- bitxor(D[d[i']],D[d[i]])
        for irow = row+1:ROWS
            if (A(irow,row) == 1)
                A(irow,:) = bitxor( A(irow,:), A(row,:) );
                Symbol(d(irow)) = bitxor( Symbol(d(irow)), Symbol(d(row)) );
            elseif (A(irow,row) ~= 0)
                % Multiply pivot_row by A(irow,row) and bitxor it to irow
                % Multiply pivot_row with the multiplicative inverse of the
                % leading coefficient of it (A(row,col)) and then by the leading
                % coefficient of the current row (A(irow,col)). Then add
                % this row to the row A(irow,:)
                coeff = rfc6330_findcoeff(A(row,col),1);
                coeff = rfc6330_gfmult(coeff,A(irow,col));
                A(irow,:) = bitxor( A(irow,:),...
                    rfc6330_gfmult_elementwise(coeff*ones(1,COLS),A(row,:)) );
                Symbol(d(irow)) = bitxor( Symbol(d(irow)),...
                    rfc6330_gfmult(coeff,Symbol(d(row))) );
            end
        end
    end
    ii = ii + 1;
    uu = uu + min_weight - 1;
end
X = X(1:ii,1:ii);
I_mat = A(1:ii,1:ii);
U = A(:,COLS-uu+1:COLS);
U_upper = U(1:ii,:);
U_lower = U(ii+1:ROWS,:);
%
% Second phase Gaussian elimination on U_lower
%
% offset = length(I_mat);
IntermediateSymbols = zeros(1,COLS);
[returnSymbols U_lower] = rfc6330_gaussian(U_lower,Symbol(d(ii+1:ROWS)));
% U_lower
% returnSymbols
% IntermediateSymbols(c(ii+1:COLS))
if (returnSymbols == 0)
    disp('Decoding failed!')
    return
end
if length(returnSymbols)>1,
    returnSymbols = returnSymbols(1:length(U_lower(1,:)));
end;
IntermediateSymbols(c(ii+1:COLS)) = returnSymbols;
Symbol(d(ii+1:COLS)) = returnSymbols;
%
% Reconstruct matrix A
%
A = [I_mat U_upper; zeros(COLS-ii,ii) U_lower];
%
% Third Phase - Make U_upper matrix to sparse using matrix X
%
A(1:ii,:) = rfc6330_gfMatrixMult(X,A(1:ii,:));
size(X);
size(Symbol(d(1:ii)).');
s = rfc6330_gfMatrixMult(X,Symbol(d(1:ii)).');
Symbol(d(1:ii)) = s.';
%
% Fourth Phase - Use U_lower to zero out U_upper
%
[U_up_ROWS U_up_COLS] = size(U_upper);
for urow = 1:U_up_ROWS
    for ucol = 1:U_up_COLS
        if (A(urow,ucol+ii) ~= 0)
            coeff = A(urow,ucol+ii);
            mult_row = rfc6330_gfmult_elementwise(coeff*ones(1,COLS),A(ucol+ii,:));
            A(urow,:) = bitxor( A(urow,:), mult_row );
            Symbol(d(urow)) = bitxor(Symbol(d(urow)),...
                rfc6330_gfmult(coeff,Symbol(d(ucol+ii))));
        end
    end
end
%[IntermediateSymbols(c) finmat] = rfc6330_gaussian(A,Symbol(d));
%
% Fifth Phase - Final tranformation of A into identity matrix
%
for jnd = 1:ii
    if (A(jnd,jnd) ~= 1)
        coeff = rfc6330_findcoeff(A(jnd,jnd),1);
        A(jnd,:) = rfc6330_gfmult_elementwise( coeff*ones(1,COLS), A(jnd,:) );
        Symbol(d(jnd)) = rfc6330_gfmult(Symbol(d(jnd)),coeff);
    end
    for col = 1:jnd-1
        if (A(jnd,col) ~= 0)
            mult_row = rfc6330_gfmult_elementwise( A(jnd,col)*ones(1,COLS),A(col,:));
            Symbol(d(jnd)) = bitxor(Symbol(d(jnd)),...
                rfc6330_gfmult(A(jnd,col),Symbol(d(col))));
            A(jnd,:) = bitxor(A(jnd,:),mult_row);
        end
    end
end
IntermediateSymbols(c(1:ii)) = Symbol(d(1:ii));
***************************************/

#include "RFC6330func.h"
#include "stdlib.h"


void mat_invert(unsigned char *IntermediateSymbols,
	params_t * Params, 
	unsigned char * A,
	unsigned char * Symbols,
	unsigned int NumSymbols)
{
	unsigned int ROWS, COLS;
	unsigned int S, H, P, L;
//	unsigned int B, W, U, K_prime, P1;
//	K_prime = Params->K_prime;
	S = Params->S;
	H = Params->H;
//	B = Params->B;
	P = Params->P;
//	W = Params->W;
	L = Params->L;
//	U = Params->U;
//	P1 = Params->P1;

	ROWS = S + H + NumSymbols;
	COLS = L;
/**********************************************
%
% Initial indices of rows and columns - keep track of row/column exchanges
%
c = 1:COLS;                             % Intermediate Symbol mapping
d = (1:ROWS)';                          % source/received symbol mapping
%
% Matrix X - It should be lower triangular at the end of the first phase
%
X = A;
ii = 0;
uu = P;

hdpc = (S+1):(S+H);
process_hdpc = false;
processed = [zeros(1,S) ones(1,H) zeros(1,ROWS-S-H)];
***********************************************/
	unsigned int *c, *d, *hdpc, *weights;
	unsigned char *X, *V;
	unsigned int ii, uu;
	bool *processed;
//	bool process_hdpc = false;

	// Allocate vectors
	c = (unsigned int *) malloc(COLS * sizeof(int));
	d = (unsigned int *) malloc(ROWS * sizeof(int));
	weights = (unsigned int *) malloc(ROWS * sizeof(int));

	X = (unsigned char *) malloc(ROWS * COLS);	
	V = (unsigned char *) malloc(ROWS * COLS);

	hdpc = (unsigned int *) malloc(H * sizeof(int));
	processed = (bool *) malloc(ROWS * sizeof(bool));

	// Initialize vectors
	for(unsigned int i = 0; i < COLS; i++) c[i] = i;
	for(unsigned int i = 0; i < ROWS; i++) d[i] = i;
	for(unsigned int i = 0; i < H; i++) hdpc[i] = S + i;
	mat_copy(X, ROWS, A, ROWS, ROWS, COLS);
	for(unsigned int i = 0; i < H; i++) processed[S+i] = true;


	ii = 0;
	uu = P;


/*************************************
while (ii + uu < COLS)
    % Choose matrix V to be tranformed into the identity matrix
    % Last step ii + uu = COLS - 1 -> ii + 1 = COLS - uu ->
    % matrix V is a column vector.
    V = A(ii+1:ROWS,ii+1:COLS-uu);
    row = ii + 1;
    [V_ROWS V_COLS] = size(V); %#ok<NASGU>
    weights = zeros(1,V_ROWS);
    for ind1 = 1:(V_ROWS)
        weights(ind1) = length(find(V(ind1,:)));
    end
    if (sum(weights) == 0)
        error(['Matrix V in phase 1 all zero! Step: ',int2str(row)])
    end
    min_weight = min(weights);
    r = 0;
    pivot_row = [];
**************************************/
	while( (ii + uu) < COLS)
	{
		int V_ROWS, V_COLS;
		V_ROWS = ROWS - ii;
		V_COLS = COLS - uu - ii;
		mat_copy(V, V_COLS, A + ii * COLS, COLS, V_ROWS, V_COLS);
//		row = ii;

		// Calculate how many non-zero elements in each row of V
		for(int i = 0; i < V_ROWS; i++)
			weights[i] = count_nonzeroes(V + i * V_COLS, V_COLS);
		// !TODO:  add check for all zeroes!!!!


	}

}

unsigned int count_nonzeroes(unsigned char *Vector, unsigned int Size)
{
	unsigned int Result = 0;
	while(Size--)
		if(*Vector++) Result++;
	return Result;
}
