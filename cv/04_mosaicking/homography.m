function h = homography(p1, p2)
    a = zeros(2*height(p1),9);

    for i=1:height(p1)
        temp = zeros(2,9);
        temp(1,1:3) = [-p1(i,1) -p1(i,2) -1];
        temp(2,4:6) = [-p1(i,1) -p1(i,2) -1];
        temp(2,1:3) = [0 0 0];
        temp(1,4:6) = [0 0 0];
        temp(1,7:9) = p2(i,1) * [p1(i,1) p1(i,2) 1];
        temp(2,7:9) = p2(i,2) * [p1(i,1) p1(i,2) 1];
        a(2*i-1:2*i, :) = temp;
    end
    [~,~,V] = svd(a);
    n = width(V);
    h = reshape(V(:,n),[3,3])';
end
