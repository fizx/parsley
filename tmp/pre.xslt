<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="xml" indent="yes"/>
<xsl:strip-space elements="*"/>
<xsl:key name="pos" match="//*" use="count(preceding::*) + count(ancestor::*)"/>
	<xsl:template match="@*|node()">
	  <xsl:copy>
			<xsl:attribute name="preceding"><xsl:value-of select="key(pos, .)" /></xsl:attribute>
	    <xsl:apply-templates select="node()"/>
	  </xsl:copy>
	</xsl:template>
	<xsl:template match="@*">
	  <xsl:copy>
	    <xsl:apply-templates select="@*|node()"/>
	  </xsl:copy>
	</xsl:template>
</xsl:stylesheet>
