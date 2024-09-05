function w = reg_sto(x, y, eta)
    w = [0;1;0.5];
    N = height(x);
    gm = 10;
    imax = 10000; % max iterations
    % perform regression
    for k = 1:imax
        i = randi([1 N]);
        th = power(1+exp(y(i) * w' * x(i,:)'),-1);
        g = y(i) * x(i,:)' * th;
        v = -(sum(g)/N)';
        w = w - eta*v;
        if abs(g-gm) < 0.0001
            return;
        end
        gm = g;
        % g_plot(x, y, w);
    end
end