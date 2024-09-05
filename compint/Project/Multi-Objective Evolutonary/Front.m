function [r, ri] = Front(P, idx)
    if size(P) == 1
        r = P;
        ri = idx;
    else
        split = floor(height(P)/2.0);
        [T, Ti] = Front(P(1:split), idx(1:split));
        [B, Bi] = Front(P(split+1:height(P)), idx(split+1:width(idx)));
        M = T;
        Mi = Ti;
        for i = 1:size(B)
            if TBCMP(T, B(i))
                M = vertcat(M, B(i));
                Mi = horzcat(Mi, Bi(i));
            end
        end
        r = M;
        ri = Mi;
    end
end