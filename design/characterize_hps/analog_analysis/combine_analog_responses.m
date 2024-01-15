% Copyright (C) 2024 Greg C. Zweigle
%
% This program is free software: you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
% 
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with this program. If not, see <https://www.gnu.org/licenses/>.
%
% Location of documentation, code, and design:
% https://github.com/gzweigle/DIY-Grand-Digital-Piano
%
% combine_analog_responses.m
%
% Run from within Matlab or Octave.
%

clear;

% Resampling function.
function cny_out = linear_resample(cny_in, N_out)
  N_in = length(cny_in);
  if (N_in == N_out),
    cny_out = cny_in;
  else
    ni = 1;
    cny_out(1) = cny_in(1);
    for no = 2:N_out-1,
      ni = ni + N_in/N_out;
      i = floor(ni);
      r = ni - i;
      a = cny_in(i);
      b = cny_in(i + 1);
      cny_out(no) = a + r*(b - a);
    end;
    cny_out(N_out) = cny_in(N_in);
  end;
endfunction

% Results from generate_analog_response.m
% Edit this list to match name of those results.
% Following is for example.
load ../data/cny0.txt;
load ../data/cny1.txt;
load ../data/cny2.txt;
load ../data/cny3.txt;
load ../data/cny4.txt;

% Find the shortest length data.
N0 = length(cny0);
N1 = length(cny1);
N2 = length(cny2);
N3 = length(cny3);
N4 = length(cny4);
N = min([N0 N1 N2 N3, N4]);

% Resample to the shortest.
cny0r = linear_resample(cny0, N);
cny1r = linear_resample(cny1, N);
cny2r = linear_resample(cny2, N);
cny3r = linear_resample(cny3, N);
cny4r = linear_resample(cny4, N);

figure(2);
subplot(2,1,1);
% Resampled results.
plot(1:N0,cny0,1:N1,cny1,1:N2,cny2,1:N3,cny3,1:N4,cny4);
grid;
subplot(2,1,2);
% Original data.
plot(1:N,cny0r,1:N,cny1r,1:N,cny2r,1:N,cny3r,1:N,cny4r);
grid;