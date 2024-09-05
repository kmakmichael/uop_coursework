function e = s_err(x, y, w)
    n = height(x);
    s = zeros(n,1);
    err = zeros(n,1);
    for i=1:n
       s(i) = w'*[1 x(i,:)]';
       err(i) = 0.5 * ((s(i) - y(i))^2);
    end
    e = mean(err);
end