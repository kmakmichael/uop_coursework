function [tce,tse,vce,vse,g] = validation(xd, yd, F, l)
    i = randperm(height(xd));
    vsize = floor(height(xd)/F);
    tce = zeros(F,1);
    tse = zeros(F,1);
    vce = zeros(F,1);
    vse = zeros(F,1);
    for f=1:F
        sel = i(vsize*(f-1)+1:vsize*f);
        xv = xd(sel,:);
        yv = yd(sel);
        xt = xd(setdiff(i, sel),:);
        yt = yd(setdiff(i, sel));
        g = linreg(xt,yt,l);
        tce(f) = c_err(xt, yt, g);
        tse(f) = s_err(xt, yt, g);
        vce(f) = c_err(xv, yv, g);
        vse(f) = s_err(xv, yv, g);
    end
end