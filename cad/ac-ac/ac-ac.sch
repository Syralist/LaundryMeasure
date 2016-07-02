EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:diy_connectors
LIBS:con-jack
LIBS:m-pad-2.1
LIBS:ac-ac-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L CON-HEADPHONE2-1 J2
U 1 1 576ACF68
P 2150 1450
F 0 "J2" H 1810 1770 50  0000 L BNN
F 1 "CON-HEADPHONE2-1" H 1820 1120 50  0000 L BNN
F 2 "trs_3:TRS_3.5" H 2150 1600 50  0001 C CNN
F 3 "" H 2150 1450 60  0000 C CNN
	1    2150 1450
	-1   0    0    1   
$EndComp
$Comp
L R R0
U 1 1 576E638A
P 4250 1550
F 0 "R0" V 4330 1550 50  0000 C CNN
F 1 "150" V 4250 1550 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM7mm" V 4180 1550 50  0001 C CNN
F 3 "" H 4250 1550 50  0000 C CNN
	1    4250 1550
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 576E655B
P 4900 1550
F 0 "R1" V 4980 1550 50  0000 C CNN
F 1 "10k" V 4900 1550 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM7mm" V 4830 1550 50  0001 C CNN
F 3 "" H 4900 1550 50  0000 C CNN
	1    4900 1550
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 576E65FA
P 4900 2050
F 0 "R2" V 4980 2050 50  0000 C CNN
F 1 "10k" V 4900 2050 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM7mm" V 4830 2050 50  0001 C CNN
F 3 "" H 4900 2050 50  0000 C CNN
	1    4900 2050
	1    0    0    -1  
$EndComp
$Comp
L CP C1
U 1 1 576E68B2
P 4450 2050
F 0 "C1" H 4475 2150 50  0000 L CNN
F 1 "10u" H 4475 1950 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Radial_D5_L11_P2" H 4488 1900 50  0001 C CNN
F 3 "" H 4450 2050 50  0000 C CNN
	1    4450 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	2650 1250 5800 1250
Wire Wire Line
	4250 1250 4250 1400
Wire Wire Line
	2650 1350 2900 1350
Wire Wire Line
	2900 1350 2900 1700
Wire Wire Line
	2900 1700 4900 1700
Wire Wire Line
	4450 1700 4450 1900
Connection ~ 4250 1700
Wire Wire Line
	4450 2200 4450 2450
Wire Wire Line
	4450 2450 5800 2450
Wire Wire Line
	4900 2450 4900 2200
Wire Wire Line
	4900 1700 4900 1900
Connection ~ 4450 1700
$Comp
L CONN_01X03 J1
U 1 1 576E6B77
P 6000 1800
F 0 "J1" H 6000 2000 50  0000 C CNN
F 1 "CONN_01X03" V 6100 1800 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Angled_1x03" H 6000 1800 50  0001 C CNN
F 3 "" H 6000 1800 50  0000 C CNN
	1    6000 1800
	1    0    0    -1  
$EndComp
Connection ~ 4900 1800
Wire Wire Line
	5800 2450 5800 1900
Connection ~ 4900 2450
Wire Wire Line
	5800 1800 5200 1800
Wire Wire Line
	5200 1800 5200 1400
Wire Wire Line
	5200 1400 4900 1400
Wire Wire Line
	5800 1250 5800 1700
Connection ~ 4250 1250
$EndSCHEMATC
