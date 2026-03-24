function out = rotz(ang)
    out = [clean_cos(ang), -clean_sin(ang), 0;
           clean_sin(ang), clean_cos(ang), 0;
           0, 0, 1];
end