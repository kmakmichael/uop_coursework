function cmp = TBCMP(T, b)
    cmp = true;
    for j = 1:size(T)
        if (Dominates(T(j), b))
            cmp = false;
            return;
        end
    end
end