xt=11.5;
yt=13.8;
zt=6.6;
dj=5.8;
hj=3.1;
px=1;
py=0.1;
pz=3.5;

//gehäuse
cube([xt,yt,zt]);
//anschluss
translate([xt/2,0,zt/2])
rotate([90,0,0])
cylinder(r=dj/2,h=hj);
//pins
//pin 1
translate([5.75,1.4,-pz/2])
rotate([0,0,0])
cube([px,py,pz],center=true);
//pin 2
translate([10.5,9.6,-pz/2])
rotate([0,0,90])
cube([px,py,pz],center=true);
//pin 2
translate([10.5,9.6,-pz/2])
rotate([0,0,90])
cube([px,py,pz],center=true);
