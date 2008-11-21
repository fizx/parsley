<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" >
 	<xsl:output method="xml" indent="yes"/>
 	<xsl:strip-space elements="*"/>

	<xsl:template match="/">
		<root>
			<xsl:value-of select="set:difference(//p, *[@id='search_near'])" /> 
		</root>
	</xsl:template>
</xsl:stylesheet>