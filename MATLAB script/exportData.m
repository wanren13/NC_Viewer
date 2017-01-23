clear;

files = '../Dorch_12_0002.nc';

%% Read Data (same for every file)
% Open the first file
ncid = netcdf.open(files, 'NOWRITE');

latid  = netcdf.inqVarID(ncid, 'lat');
lonid  = netcdf.inqVarID(ncid, 'lon');
zetaid = netcdf.inqVarID(ncid, 'zeta');
said   = netcdf.inqVarID(ncid, 'salinity');
tempid = netcdf.inqVarID(ncid, 'temp');
vtid   = netcdf.inqVarID(ncid, 'nv');
hid    = netcdf.inqVarID(ncid, 'h');
timeid = netcdf.inqVarID(ncid, 'Times');

% get vertices coordinates
lat   = netcdf.getVar(ncid, latid);
lon   = netcdf.getVar(ncid, lonid);
zeta  = netcdf.getVar(ncid, zetaid);
sa    = netcdf.getVar(ncid, said);
temp  = netcdf.getVar(ncid, tempid);
vt    = netcdf.getVar(ncid, vtid);
h     = netcdf.getVar(ncid, hid);
t = netcdf.getVar(ncid, timeid);

times = 100;
start = 1;

fileID = fopen('info.txt', 'w');
fprintf(fileID, '%d\r\n', times-start+1);
fclose(fileID);

fileID = fopen('lat.txt', 'w');
for i=1:length(lat)
    fprintf(fileID, '%3.10f \r\n', lat(i));
end
fclose(fileID);

fileID = fopen('lon.txt', 'w');
for i=1:length(lon)
    fprintf(fileID, '%3.10f \r\n', lon(i));
end
fclose(fileID);

fileID = fopen('h.txt', 'w');
for i=1:length(h)
    fprintf(fileID, '%3.10f \r\n', h(i));
end
fclose(fileID);

% fileID = fopen('zeta.txt', 'w');
% for i=1:size(zeta,2)
%     for j=1:size(zeta,1)
%         fprintf(fileID, '%3.10f \r\n', zeta(j,i));
%     end
% end
% fclose(fileID);

fileID = fopen('zeta.txt', 'w');
for j=start:times
    for i=1:size(zeta,1)
        fprintf(fileID, '%3.10f \r\n', zeta(i,j));
    end
end
fclose(fileID);

fileID = fopen('temp.txt', 'w');
for j=start:times
    for i=1:size(temp,1)
        fprintf(fileID, '%3.10f \r\n', temp(i,j));
    end
end
fclose(fileID);

fileID = fopen('salinity.txt', 'w');
for j=start:times
    for i=1:size(sa,1)
        fprintf(fileID, '%3.10f \r\n', sa(i,j));
    end
end
fclose(fileID);


fileID = fopen('vt.txt', 'w');
for i=1:length(vt)
    fprintf(fileID, '%d %d %d \r\n', vt(i,1), vt(i,2), vt(i,3));
end
fclose(fileID);



%{
save('lon.txt','lon','-ascii', '-single');
save('lat.txt','lat','-ascii');
save('zeta.txt','zeta','-ascii');
%}