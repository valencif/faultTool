%% Check HW from template

filename= 'VAR/PAR/MeanTemplate.txt';
templateV = dlmread(filename);

x=0:1:length(templateV)-1;

xbin=de2bi(x);
xhw=sum(xbin,2);

plot(xhw, templateV, '*');