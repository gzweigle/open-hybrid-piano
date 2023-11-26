% To run this code:
% Install Matlab or Octave.
% Run Matlab or Octave.
% Navigate to this directory on computer.
% Run the python code get_ethernet_data.py
% In Matlab or Ocative type: plot_ethernet_data
clear;
load ethernet_data_hammer.txt;
load ethernet_data_damper.txt;
subplot(2,1,1);
plot(ethernet_data_hammer);
grid;
subplot(2,1,2);
plot(ethernet_data_damper);
grid;