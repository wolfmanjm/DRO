use <myLibs.scad>

mm= 25.4;
mst= 1.8; // thickness of magnetic strip
msw= 10; // width of magnetic strip
carriage_length= 30;
rail_length= 3*mm + carriage_length;
rail_width= 20;
rail_depth= 5;
rail_dia= 1/4*mm;
rail_height= 35;
pcb_width= 35.4;
sep= rail_width/2 + 13;
end_width= 58;
offset= 10-5+2.2+1.02;

//rotate([90, 0, 0])
difference() {
	union() {
		%translate([0, 0, rail_height/2-3])  carriage();
		rail();
	}
	#translate([sep, rail_length/2+6, rail_height/2]) rotate([90, 0, 0]) hole(rail_dia, rail_length+10, 0.2);
	#translate([-sep, rail_length/2+6, rail_height/2]) rotate([90, 0, 0]) hole(rail_dia, rail_length+10, 0.2);
}

module board(ht) {
	chipoffset= 9.4;
	translate([-pcb_width-0.4, -30-1.7,ht])  import("magnetic-linear-encoder.dxf");
	// chip
	translate([chipoffset-4.4/2, 0, ht-0.9])  cube(size=[4.4,6.5,2.71], center=true);
	// socket
	translate([-pcb_width/2+8, 0, ht+3.0])  cube(size=[6,7,5], center=true);
}

module carriage() {
	l= carriage_length;
	w= pcb_width;
	h= 3;
	cw= end_width;

	%board(-9.3 + 0.4); // <= 0.4mm above strip
	difference() {
		union() {
	    	cube(size=[cw, l, h], center=true);
	    	translate([-cw/2, -l/2, 0])  cube(size=[rail_dia+6, l, 8], center=false);
	    	translate([cw/2-(rail_dia+6), -l/2, 0])  cube(size=[rail_dia+6, l, 8], center=false);
	    }
	    #translate([-w/2+3.5, -l/2+3.5, -7])  hole(3, 10);
	    translate([w/2-3.5, -l/2+3.5, -7])   hole(3, 10);
	    translate([-w/2+3.5, l/2-3.5, -7])   hole(3, 10);
	    translate([w/2-3.5, l/2-3.5, -7])    hole(3, 10);
	}
}

module rail() {
	l= rail_length;
	w= rail_width-8;
	t= rail_depth;

	difference() {
		union() {
			translate([offset, 0, 0]) cube(size=[w, l, t], center=true);
			slides();
		}
		#translate([offset, 0, t/2-mst/4+0.05])  cube(size=[msw, l+10, mst], center=true);
	}
}

module slides() {
	l= rail_length;
	w= end_width;
	t= 5;
	h= rail_height-6;
	translate([0, l/2, h/2-rail_depth/2])  cube(size=[w, t, h], center=true);
	translate([0, -l/2, h/2-rail_depth/2])  cube(size=[w, t, h], center=true);
}
