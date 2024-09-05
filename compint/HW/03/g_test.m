function a = pla_test(x, y, g)
    a = 0;
    for k = 1:height(x)
        if (sign(dot(g,x(k,:))) == y(k))
            a = a + 1;
        end
    end
    a = a / height(x);
end