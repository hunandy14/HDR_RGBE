function [bT] = bias(t ,b)
bT = t .^ ( log(b) / log(0.5) );
end