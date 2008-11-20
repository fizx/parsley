<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:func="http://exslt.org/functions" xmlns:str="http://exslt.org/strings" extension-element-prefixes="func str" xmlns:custom="http://kylemaxwell.com/custom">
 	<xsl:output method="text" />
 	<xsl:strip-space elements="*"/>

	<xsl:template match="/">
		<xsl:value-of select="custom:jsonize(//h1)" />
	</xsl:template>

	<xsl:variable name="nl">
		<xsl:text>
</xsl:text>
	</xsl:variable>
	<xsl:variable name="sq"><xsl:text>'</xsl:text></xsl:variable>
	<xsl:variable name="tab">	
		<xsl:text>	</xsl:text>
	</xsl:variable>


	<func:function name="custom:jsonize">
	   <xsl:param name="input" />
		 <func:result select="str:replace($a, $nl, '\n')" />
	</func:function>
</xsl:stylesheet>