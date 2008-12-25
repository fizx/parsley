<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:dexter="http://kylemaxwell.com/dexter" xmlns:str="http://exslt.org/strings" xmlns:set="http://exslt.org/sets" xmlns:math="http://exslt.org/math" xmlns:func="http://exslt.org/functions" xmlns:user="http://kylemaxwell.com/dexter/user-functions" xmlns:dyn="http://exslt.org/dynamic" xmlns:date="http://exslt.org/dates-and-times" xmlns:exsl="http://exslt.org/common" xmlns:saxon="http://icl.com/saxon" version="1.0" extension-element-prefixes="str math set func dyn exsl saxon user date">
	
	<xsl:output method="xml" indent="yes"/>
	<xsl:strip-space elements="*"/>
	
	<!-- 	
		{
			"page": [{
				"title": "h1",
				"nav": ["ul#nav li"],
				"post(#posts li)": [{
					"title": "h2",
					"paras": ["p"],
					"comments": [{
						"title": "#comments h3",
						"paras": ["#comments p"]
					}]
				}]
			}]
		}
	-->

	<xsl:template match="node()">
	  <xsl:copy>
			<xsl:attribute name="index"><xsl:value-of select="concat(count(set:intersection(following::*, //h1)), '-',
			            count(set:intersection(following::*, //*[@id='posts']//li//h2)), '-',
									count(set:intersection(following::*, //*[@id='posts']//li//*[@id='comments']//h3)))" /></xsl:attribute>
	    <xsl:apply-templates select="@*|node()"/>
	  </xsl:copy>
	</xsl:template>
	
	<xsl:template match="@*">
	  <xsl:copy>
	    <xsl:apply-templates select="@*|node()"/>
	  </xsl:copy>
	</xsl:template>
	
	
</xsl:stylesheet>


