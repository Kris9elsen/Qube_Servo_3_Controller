function out = rotx(ang)
    out = [1, 0, 0;
           0, clean_cos(ang), -clean_sin(ang);
           0, clean_sin(ang), clean_cos(ang)];
end