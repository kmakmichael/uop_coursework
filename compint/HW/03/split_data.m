function [x_t,y_t,x,y] = split_data(xd,yd,N)
    x_t = zeros(2*N,3);
    y_t = zeros(2*N,1);
    x = zeros(height(xd)-2*N,3);
    y = zeros(height(xd)-2*N,1);
    % add ones
    for n = 1:N
        r = randi(500);
        x_t(n,1) = 1;
        x_t(n,2) = xd(r,1);
        x_t(n,3) = xd(r,2);
        y_t(n) = yd(r);
        xd(r,:) = [];
        yd(r) = [];
    end
    % add non-ones
    for n = N+1:2*N
        r = randi([501-N,height(xd)]);
        x_t(n,1) = 1;
        x_t(n,2) = xd(r,1);
        x_t(n,3) = xd(r,2);
        y_t(n) = yd(r);
        xd(r,:) = [];
        yd(r) = [];
    end
    x = horzcat(ones(height(xd),1),xd);
    y = yd;
end