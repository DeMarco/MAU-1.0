<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="9.1.0">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting keepoldvectorfont="yes"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="yes" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="yes" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="yes" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="yes" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="yes" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="yes" active="no"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="yes" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="yes" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="yes" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="yes" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="yes" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="yes" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="yes" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="yes" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="yes" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="yes" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="yes" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="yes" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="yes" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="yes" active="no"/>
<layer number="51" name="tDocu" color="7" fill="1" visible="yes" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="yes" active="no"/>
<layer number="90" name="Modules" color="5" fill="1" visible="yes" active="yes"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="altimeter">
<packages>
<package name="ISP_CONN_6_M_DRILL13">
<pad name="1" x="-1.524" y="7.62" drill="0.8" shape="square"/>
<pad name="2" x="0" y="5.08" drill="0.8"/>
<pad name="3" x="0" y="2.54" drill="0.8"/>
<pad name="4" x="0" y="0" drill="0.8"/>
<pad name="5" x="0" y="-2.54" drill="0.8"/>
<pad name="6" x="0" y="-5.08" drill="0.8"/>
</package>
<package name="ISP_CONN_6_M_DRILL16">
<pad name="1" x="-1.524" y="7.62" drill="1" shape="square"/>
<pad name="2" x="0" y="5.08" drill="1"/>
<pad name="3" x="0" y="2.54" drill="1"/>
<pad name="4" x="0" y="0" drill="1"/>
<pad name="5" x="0" y="-2.54" drill="1"/>
<pad name="6" x="0" y="-5.08" drill="1"/>
</package>
</packages>
<symbols>
<symbol name="ISP_CONN_6">
<pin name="MISO" x="-5.08" y="12.7" length="middle"/>
<pin name="VCC" x="-5.08" y="7.62" length="middle"/>
<pin name="SCK" x="-5.08" y="2.54" length="middle"/>
<pin name="MOSI" x="-5.08" y="-2.54" length="middle"/>
<pin name="RST" x="-5.08" y="-7.62" length="middle"/>
<pin name="GND" x="-5.08" y="-12.7" length="middle"/>
<rectangle x1="-2.54" y1="2.286" x2="0" y2="2.794" layer="94"/>
<rectangle x1="-2.54" y1="7.366" x2="0" y2="7.874" layer="94"/>
<rectangle x1="-2.54" y1="12.446" x2="0" y2="12.954" layer="94"/>
<rectangle x1="-2.54" y1="-2.794" x2="0" y2="-2.286" layer="94"/>
<rectangle x1="-2.54" y1="-7.874" x2="0" y2="-7.366" layer="94"/>
<rectangle x1="-2.54" y1="-12.954" x2="0" y2="-12.446" layer="94"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="ISP_CONN_6_M">
<gates>
<gate name="G$1" symbol="ISP_CONN_6" x="-10.16" y="0"/>
</gates>
<devices>
<device name="" package="ISP_CONN_6_M_DRILL13">
<connects>
<connect gate="G$1" pin="GND" pad="6"/>
<connect gate="G$1" pin="MISO" pad="1"/>
<connect gate="G$1" pin="MOSI" pad="4"/>
<connect gate="G$1" pin="RST" pad="5"/>
<connect gate="G$1" pin="SCK" pad="3"/>
<connect gate="G$1" pin="VCC" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="DRILL16" package="ISP_CONN_6_M_DRILL16">
<connects>
<connect gate="G$1" pin="GND" pad="6"/>
<connect gate="G$1" pin="MISO" pad="1"/>
<connect gate="G$1" pin="MOSI" pad="4"/>
<connect gate="G$1" pin="RST" pad="5"/>
<connect gate="G$1" pin="SCK" pad="3"/>
<connect gate="G$1" pin="VCC" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="con-3m">
<description>&lt;b&gt;3M Connectors&lt;/b&gt;&lt;p&gt;
PCMCIA-CompactFlash Connectors&lt;p&gt;
Zero Insertion Force Socked&lt;p&gt;
&lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
<package name="PAK100/2500-10">
<description>&lt;b&gt;3M (TM) Pak 100 4-Wall Header&lt;/b&gt; Straight&lt;p&gt;
Source: 3M</description>
<wire x1="-10" y1="4.2" x2="10" y2="4.2" width="0.2032" layer="21"/>
<wire x1="10" y1="4.2" x2="10" y2="-4.2" width="0.2032" layer="21"/>
<wire x1="10" y1="-4.2" x2="5.938" y2="-4.2" width="0.2032" layer="21"/>
<wire x1="5.938" y1="-4.2" x2="5.938" y2="-3.9" width="0.2032" layer="21"/>
<wire x1="5.938" y1="-3.9" x2="4.459" y2="-3.9" width="0.2032" layer="21"/>
<wire x1="4.459" y1="-3.9" x2="4.459" y2="-4.2" width="0.2032" layer="21"/>
<wire x1="4.459" y1="-4.2" x2="1.883" y2="-4.2" width="0.2032" layer="21"/>
<wire x1="1.883" y1="-4.2" x2="1.883" y2="-2.65" width="0.2032" layer="21"/>
<wire x1="1.883" y1="-2.65" x2="-1.883" y2="-2.65" width="0.2032" layer="21"/>
<wire x1="-1.883" y1="-2.65" x2="-1.883" y2="-4.2" width="0.2032" layer="21"/>
<wire x1="1.883" y1="-4.2" x2="-1.883" y2="-4.2" width="0.2032" layer="21"/>
<wire x1="-1.883" y1="-4.2" x2="-10" y2="-4.2" width="0.2032" layer="21"/>
<wire x1="-10" y1="-4.2" x2="-10" y2="4.2" width="0.2032" layer="21"/>
<wire x1="-8.875" y1="3.275" x2="8.875" y2="3.275" width="0.2032" layer="21"/>
<wire x1="8.875" y1="3.275" x2="8.875" y2="-3.275" width="0.2032" layer="21"/>
<wire x1="8.875" y1="-3.275" x2="1.883" y2="-3.275" width="0.2032" layer="21"/>
<wire x1="-1.883" y1="-3.275" x2="-8.875" y2="-3.275" width="0.2032" layer="21"/>
<wire x1="-8.875" y1="-3.275" x2="-8.875" y2="3.275" width="0.2032" layer="21"/>
<pad name="1" x="-5.08" y="-1.27" drill="1" diameter="1.4224"/>
<pad name="2" x="-5.08" y="1.27" drill="1" diameter="1.4224"/>
<pad name="3" x="-2.54" y="-1.27" drill="1" diameter="1.4224"/>
<pad name="4" x="-2.54" y="1.27" drill="1" diameter="1.4224"/>
<pad name="5" x="0" y="-1.27" drill="1" diameter="1.4224"/>
<pad name="6" x="0" y="1.27" drill="1" diameter="1.4224"/>
<pad name="7" x="2.54" y="-1.27" drill="1" diameter="1.4224"/>
<pad name="8" x="2.54" y="1.27" drill="1" diameter="1.4224"/>
<pad name="9" x="5.08" y="-1.27" drill="1" diameter="1.4224"/>
<pad name="10" x="5.08" y="1.27" drill="1" diameter="1.4224"/>
<text x="-10.16" y="4.572" size="1.27" layer="25">&gt;NAME</text>
<text x="-2.54" y="4.572" size="1.27" layer="27">&gt;VALUE</text>
</package>
<package name="PAK100/2500-5-10">
<description>&lt;b&gt;3M (TM) Pak 100 4-Wall Header&lt;/b&gt; Right Angle&lt;p&gt;
Source: 3M</description>
<wire x1="11.27" y1="1.875" x2="11.27" y2="11.075" width="0.2032" layer="21"/>
<wire x1="11.27" y1="11.075" x2="5.938" y2="11.075" width="0.2032" layer="21"/>
<wire x1="4.459" y1="11.075" x2="5.938" y2="11.075" width="0.2032" layer="21"/>
<wire x1="5.938" y1="11.075" x2="5.938" y2="10.105" width="0.2032" layer="21"/>
<wire x1="5.938" y1="10.105" x2="4.459" y2="10.105" width="0.2032" layer="21"/>
<wire x1="4.459" y1="10.105" x2="4.459" y2="11.075" width="0.2032" layer="21"/>
<wire x1="4.459" y1="11.075" x2="1.883" y2="11.075" width="0.2032" layer="21"/>
<wire x1="1.883" y1="11.075" x2="1.883" y2="4.01" width="0.2032" layer="21"/>
<wire x1="1.883" y1="4.01" x2="-1.883" y2="4.01" width="0.2032" layer="21"/>
<wire x1="-1.883" y1="4.01" x2="-1.883" y2="11.075" width="0.2032" layer="21"/>
<wire x1="1.883" y1="11.075" x2="-1.883" y2="11.075" width="0.2032" layer="21"/>
<wire x1="-1.883" y1="11.075" x2="-10" y2="11.075" width="0.2032" layer="21"/>
<wire x1="-10" y1="11.075" x2="-10" y2="1.875" width="0.2032" layer="21"/>
<wire x1="-6.456" y1="1.875" x2="-10" y2="1.875" width="0.2032" layer="21"/>
<wire x1="7.709" y1="1.875" x2="-6.452" y2="1.875" width="0.2032" layer="51"/>
<wire x1="11.27" y1="1.875" x2="7.713" y2="1.875" width="0.2032" layer="21"/>
<pad name="1" x="-5.08" y="-1.27" drill="1" diameter="1.4224"/>
<pad name="2" x="-5.08" y="1.27" drill="1" diameter="1.4224"/>
<pad name="3" x="-2.54" y="-1.27" drill="1" diameter="1.4224"/>
<pad name="4" x="-2.54" y="1.27" drill="1" diameter="1.4224"/>
<pad name="5" x="0" y="-1.27" drill="1" diameter="1.4224"/>
<pad name="6" x="0" y="1.27" drill="1" diameter="1.4224"/>
<pad name="7" x="2.54" y="-1.27" drill="1" diameter="1.4224"/>
<pad name="8" x="2.54" y="1.27" drill="1" diameter="1.4224"/>
<pad name="9" x="5.08" y="-1.27" drill="1" diameter="1.4224"/>
<pad name="10" x="5.08" y="1.27" drill="1" diameter="1.4224"/>
<text x="-10.16" y="-3.81" size="1.27" layer="25">&gt;NAME</text>
<text x="-2.54" y="-3.81" size="1.27" layer="27">&gt;VALUE</text>
<polygon width="0.2032" layer="21">
<vertex x="-6.36" y="10.945"/>
<vertex x="-3.81" y="10.945"/>
<vertex x="-5.085" y="8.37"/>
</polygon>
</package>
</packages>
<symbols>
<symbol name="PINV">
<text x="-1.27" y="0.889" size="1.778" layer="95" rot="R180">&gt;NAME</text>
<text x="-3.81" y="2.667" size="1.778" layer="96">&gt;VALUE</text>
<rectangle x1="0" y1="-0.254" x2="2.794" y2="0.254" layer="94"/>
<pin name="KL" x="5.08" y="0" visible="off" length="short" direction="pas" rot="R180"/>
</symbol>
<symbol name="PIN">
<text x="-1.27" y="0.889" size="1.778" layer="95" rot="R180">&gt;NAME</text>
<rectangle x1="0" y1="-0.254" x2="2.794" y2="0.254" layer="94"/>
<pin name="KL" x="5.08" y="0" visible="off" length="short" direction="pas" rot="R180"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="2510-" prefix="X">
<description>&lt;b&gt;3M (TM) Pak 100 4-Wall Header&lt;/b&gt;&lt;p&gt;
Source: 3M</description>
<gates>
<gate name="-1" symbol="PINV" x="0" y="0" addlevel="always"/>
<gate name="-2" symbol="PIN" x="0" y="-2.54" addlevel="always"/>
<gate name="-3" symbol="PIN" x="0" y="-5.08" addlevel="always"/>
<gate name="-4" symbol="PIN" x="0" y="-7.62" addlevel="always"/>
<gate name="-5" symbol="PIN" x="0" y="-10.16" addlevel="always"/>
<gate name="-6" symbol="PIN" x="0" y="-12.7" addlevel="always"/>
<gate name="-7" symbol="PIN" x="0" y="-15.24" addlevel="always"/>
<gate name="-8" symbol="PIN" x="0" y="-17.78" addlevel="always"/>
<gate name="-9" symbol="PIN" x="0" y="-20.32" addlevel="always"/>
<gate name="-10" symbol="PIN" x="0" y="-22.86" addlevel="always"/>
</gates>
<devices>
<device name="" package="PAK100/2500-10">
<connects>
<connect gate="-1" pin="KL" pad="1"/>
<connect gate="-10" pin="KL" pad="10"/>
<connect gate="-2" pin="KL" pad="2"/>
<connect gate="-3" pin="KL" pad="3"/>
<connect gate="-4" pin="KL" pad="4"/>
<connect gate="-5" pin="KL" pad="5"/>
<connect gate="-6" pin="KL" pad="6"/>
<connect gate="-7" pin="KL" pad="7"/>
<connect gate="-8" pin="KL" pad="8"/>
<connect gate="-9" pin="KL" pad="9"/>
</connects>
<technologies>
<technology name="">
<attribute name="MF" value="3M" constant="no"/>
<attribute name="MPN" value="2510-6002UB" constant="no"/>
<attribute name="OC_FARNELL" value="9838244" constant="no"/>
<attribute name="OC_NEWARK" value="46F4725" constant="no"/>
</technology>
</technologies>
</device>
<device name="5" package="PAK100/2500-5-10">
<connects>
<connect gate="-1" pin="KL" pad="1"/>
<connect gate="-10" pin="KL" pad="10"/>
<connect gate="-2" pin="KL" pad="2"/>
<connect gate="-3" pin="KL" pad="3"/>
<connect gate="-4" pin="KL" pad="4"/>
<connect gate="-5" pin="KL" pad="5"/>
<connect gate="-6" pin="KL" pad="6"/>
<connect gate="-7" pin="KL" pad="7"/>
<connect gate="-8" pin="KL" pad="8"/>
<connect gate="-9" pin="KL" pad="9"/>
</connects>
<technologies>
<technology name="">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="" constant="no"/>
<attribute name="OC_FARNELL" value="1788669" constant="no"/>
<attribute name="OC_NEWARK" value="unknown" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="U$1" library="altimeter" deviceset="ISP_CONN_6_M" device="DRILL16" value="ISP_CONN_6_MDRILL16"/>
<part name="X1" library="con-3m" deviceset="2510-" device=""/>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="U$1" gate="G$1" x="22.86" y="22.86"/>
<instance part="X1" gate="-1" x="-22.86" y="33.02"/>
<instance part="X1" gate="-2" x="-22.86" y="30.48"/>
<instance part="X1" gate="-3" x="-22.86" y="27.94"/>
<instance part="X1" gate="-4" x="-22.86" y="25.4"/>
<instance part="X1" gate="-5" x="-22.86" y="22.86"/>
<instance part="X1" gate="-6" x="-22.86" y="20.32"/>
<instance part="X1" gate="-7" x="-22.86" y="17.78"/>
<instance part="X1" gate="-8" x="-22.86" y="15.24"/>
<instance part="X1" gate="-9" x="-22.86" y="12.7"/>
<instance part="X1" gate="-10" x="-22.86" y="10.16"/>
</instances>
<busses>
</busses>
<nets>
<net name="N$1" class="0">
<segment>
<pinref part="X1" gate="-1" pin="KL"/>
<wire x1="-17.78" y1="33.02" x2="0" y2="33.02" width="0.1524" layer="91"/>
<wire x1="0" y1="33.02" x2="0" y2="20.32" width="0.1524" layer="91"/>
<pinref part="U$1" gate="G$1" pin="MOSI"/>
<wire x1="0" y1="20.32" x2="17.78" y2="20.32" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$2" class="0">
<segment>
<pinref part="X1" gate="-2" pin="KL"/>
<pinref part="U$1" gate="G$1" pin="VCC"/>
<wire x1="-17.78" y1="30.48" x2="17.78" y2="30.48" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$3" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="GND"/>
<wire x1="17.78" y1="10.16" x2="-2.54" y2="10.16" width="0.1524" layer="91"/>
<wire x1="-2.54" y1="10.16" x2="-2.54" y2="15.24" width="0.1524" layer="91"/>
<pinref part="X1" gate="-4" pin="KL"/>
<wire x1="-2.54" y1="15.24" x2="-2.54" y2="20.32" width="0.1524" layer="91"/>
<wire x1="-2.54" y1="20.32" x2="-2.54" y2="25.4" width="0.1524" layer="91"/>
<wire x1="-2.54" y1="25.4" x2="-17.78" y2="25.4" width="0.1524" layer="91"/>
<pinref part="X1" gate="-6" pin="KL"/>
<wire x1="-17.78" y1="20.32" x2="-2.54" y2="20.32" width="0.1524" layer="91"/>
<junction x="-2.54" y="20.32"/>
<pinref part="X1" gate="-8" pin="KL"/>
<wire x1="-17.78" y1="15.24" x2="-2.54" y2="15.24" width="0.1524" layer="91"/>
<junction x="-2.54" y="15.24"/>
<pinref part="X1" gate="-10" pin="KL"/>
<wire x1="-17.78" y1="10.16" x2="-2.54" y2="10.16" width="0.1524" layer="91"/>
<junction x="-2.54" y="10.16"/>
</segment>
</net>
<net name="N$4" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="RST"/>
<wire x1="17.78" y1="15.24" x2="7.62" y2="15.24" width="0.1524" layer="91"/>
<wire x1="7.62" y1="15.24" x2="7.62" y2="22.86" width="0.1524" layer="91"/>
<pinref part="X1" gate="-5" pin="KL"/>
<wire x1="7.62" y1="22.86" x2="-17.78" y2="22.86" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$5" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="SCK"/>
<wire x1="17.78" y1="25.4" x2="5.08" y2="25.4" width="0.1524" layer="91"/>
<wire x1="5.08" y1="25.4" x2="5.08" y2="17.78" width="0.1524" layer="91"/>
<pinref part="X1" gate="-7" pin="KL"/>
<wire x1="5.08" y1="17.78" x2="-17.78" y2="17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$6" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="MISO"/>
<wire x1="17.78" y1="35.56" x2="2.54" y2="35.56" width="0.1524" layer="91"/>
<wire x1="2.54" y1="35.56" x2="2.54" y2="12.7" width="0.1524" layer="91"/>
<pinref part="X1" gate="-9" pin="KL"/>
<wire x1="2.54" y1="12.7" x2="-17.78" y2="12.7" width="0.1524" layer="91"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
<errors>
<approved hash="101,1,-17.78,27.94,X1-3,KL,,,,"/>
</errors>
</schematic>
</drawing>
</eagle>
