function [g,iter] = pla_s(x, y, eta)
    n = size(x,1);
    y_hyp = zeros(n,1);
    g = [0;1;0.5];
    for iter = 1:1000
        % check classifications
        for k = 1:n
            y_hyp(k) = sign(dot(g,x(k,:)));
        end
        if y_hyp == y
            return;
        end
        % find a point
        r = randi(n);
        while y_hyp(r) == y(r)
            r = randi(n);
        end
        % compute s(r)
        num = abs(g(2)*x(r,2) + g(3)*x(r,3) + g(1));
        den = sqrt(g(2)*g(2) + g(3)*g(3));
        s = num/den;
        % adjust weights
        g = g + eta*(y(r)-s)*x(r,:)';
    end
end