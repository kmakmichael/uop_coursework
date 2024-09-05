function [g,iter] = pla(x, y)
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
        % update weights
        r = randi(n);
        while y_hyp(r) == y(r)
            r = randi(n);
        end
        g = g + y(r)*x(r,:)';
    end
end