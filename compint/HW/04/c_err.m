function e = c_err(x, y, w)
    e = 0;
    for k = 1:height(x)
        if (sign(dot(w,[1,x(k,:)])) == y(k))
            e = e + 1;
        end
    end
    e = e / height(x);
end