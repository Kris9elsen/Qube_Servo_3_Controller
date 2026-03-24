function y = clean_sin(x)
    tol = 1e-2;

    if isa(x, 'sym')
        % Symbolic case → use simplify instead
        y = simplify(sin(x));
    else
        % Numeric case
        y = sin(x);

        y(abs(y) < tol) = 0;
        y(abs(y - 1) < tol) = 1;
        y(abs(y + 1) < tol) = -1;
    end
end