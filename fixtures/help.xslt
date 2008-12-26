<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:dexter="http://kylemaxwell.com/dexter" xmlns:str="http://exslt.org/strings" xmlns:set="http://exslt.org/sets" xmlns:math="http://exslt.org/math" xmlns:func="http://exslt.org/functions" xmlns:user="http://kylemaxwell.com/dexter/user-functions" xmlns:dyn="http://exslt.org/dynamic" xmlns:date="http://exslt.org/dates-and-times" xmlns:exsl="http://exslt.org/common" xmlns:saxon="http://icl.com/saxon" version="1.0" extension-element-prefixes="str math set func dyn exsl saxon user date">
	
	<xsl:output method="xml" indent="yes"/>
	<xsl:strip-space elements="*"/>
	
	<xsl:template match="node()">
		<xsl:copy>
			<xsl:attribute name="index"><xsl:value-of select="concat(count(set:intersection(following::*, //h1)), '-','')" /></xsl:attribute>
			<xsl:apply-templates select="@*|node()" />
		</xsl:copy>
	</xsl:template>	
	
	<xsl:template match="@*" mode="copy">
		<xsl:copy-of select="."/>
	</xsl:template>
</xsl:stylesheet>