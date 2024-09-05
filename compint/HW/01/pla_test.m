function [x_t, y_t, g, f, i, a] = pla_test(N, R)
    i = zeros(R,1); % iterations to converge
    a = zeros(R,1); % accuracy
    for r = 1:R
        [xd, yd, f] = pla_getdata(-1, 1, 2, 11*N);
        % test set
        x = xd(N+1:11*N,:);
        y = yd(N+1:11*N,:);
        % training set
        x_t = xd(1:N,:);
        y_t = yd(1:N,:);
        % train the PLA
        [g, i(r)] = pla(x_t, y_t);
        % apply to test set
        for k = 1:10*N
            if (sign(dot(g,x(k,:))) == y(k))
                a(r) = a(r) + 1; % sum correct guesses
            end
        end
        a(r) = a(r)/(10*N); % divide to get % accuracy
    end
end