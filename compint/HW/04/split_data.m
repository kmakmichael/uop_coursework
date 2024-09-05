function [x_t,y_t,x,y] = split_data(xd,yd)
    N = height(xd)/10;
    x_t = zeros(N,9);
    y_t = zeros(N,1);
    x = zeros(9*N,9);
    y = zeros(N,1);
    % add points
    for n = 1:N
        r = randi(height(xd));
        x_t(n,:) = xd(r,:);
        y_t(n) = yd(r);
        xd(r,:) = [];
        yd(r) = [];
    end
    % shuffle test set
    for n = 1:9*N
        r = randi(height(xd));
        x(n,:) = xd(r,:);
        y(n,:) = yd(r);
        xd(r,:) = [];
        yd(r) = [];
    end
end