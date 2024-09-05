function [x_t, y_t, g, f, i, a] = pla_test_s(N, R, E)
    [xd, yd, f] = pla_getdata(-1, 1, 2, 101*N);
    % test set
    x = xd(N+1:101*N,:);
    y = yd(N+1:101*N,:);
    % training set
    x_t = xd(1:N,:);
    y_t = yd(1:N,:);
    i = zeros(R,1); % iterations to converge
    a = zeros(R,1); % accuracy
    for r = 1:R
        % train the PLA
        [g, i(r)] = pla_s(x_t, y_t, E);
        % apply to test set
        for k = 1:100*N
            if (sign(dot(g,x(k,:))) == y(k))
                a(r) = a(r) + 1; % sum correct guesses
            end
        end
        a(r) = a(r)/(100*N); % divide to get % accuracy
    end
end