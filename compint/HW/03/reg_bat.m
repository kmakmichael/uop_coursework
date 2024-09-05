function w = reg_bat(x, y, eta)
    w = [0;1;0.5];
    N = height(x);
    vm = [1;1;1];
    g = zeros(N,3);
    imax = 10000; % max iterations
    % perform regression
    for k = 1:imax
        for i = 1:N
            th = power(1+exp(y(i) * w' * x(i,:)'), -1);
            g(i,:) = y(i) * x(i,:)' * th;
        end
        v = -(sum(g)/N)';
        w = w - eta*v;
        if abs(v - vm) < 0.001
            return;
        end
        vm = v;
        % g_plot(x, y, w);
    end
end