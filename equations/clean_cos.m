function y = clean_cos(x)
    tol = 1e-2;

    if isa(x, 'sym')
        % Symbolic case → use simplify instead
        y = simplify(cos(x));
    else
        % Numeric case
        y = cos(x);

        y(abs(y) < tol) = 0;
        y(abs(y - 1) < tol) = 1;
        y(abs(y + 1) < tol) = -1;
    end
end