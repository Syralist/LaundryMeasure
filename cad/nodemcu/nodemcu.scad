$fn=50;

//Grundplatte mit Bohrungen
difference()
{
    //Grundplatte
    cube([26.5,50,1.45]);
    //Bohrungen
    translate([2.8,2.0,-0.1])
        cylinder(d=3.2,h=1.6);
    translate([23.8,2.0,-0.1])
        cylinder(d=3.2,h=1.6);
    translate([2.8,45.5,-0.1])
        cylinder(d=3.2,h=1.6);
    translate([23.8,45.5,-0.1])
        cylinder(d=3.2,h=1.6);
}

//Usb-Anschluss
translate([9.7,-1.0,1.45])
    cube([7.5,5.6,2.7]);

//ESP-Platte
translate([5.3,24.0,1.45])
    cube([16.0,24.4,1.3]);

//ESP-Chip
translate([7.4,25.5,1.45+1.3])
    cube([12.0,15.0,2.0]);

//Abstandhalter
translate([0.8,4.5,-2.5])
    cube([2.45,38.1,2.5]);
translate([23.8,4.5,-2.5])
    cube([2.45,38.1,2.5]);

//Pins
for(i = [0:14])
{
    translate([1.7+0.3,6.0+i*2.54,-8.3])
        cylinder(d=0.6,h=11.2);
    translate([24.6+0.3,6.0+i*2.54,-8.3])
        cylinder(d=0.6,h=11.2);
}






