<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:dexter="http://kylemaxwell.com/dexter" xmlns:str="http://exslt.org/strings" xmlns:set="http://exslt.org/sets" xmlns:math="http://exslt.org/math" xmlns:func="http://exslt.org/functions" xmlns:user="http://kylemaxwell.com/dexter/user-functions" xmlns:dyn="http://exslt.org/dynamic" xmlns:date="http://exslt.org/dates-and-times" xmlns:exsl="http://exslt.org/common" xmlns:saxon="http://icl.com/saxon" version="1.0" extension-element-prefixes="str math set func dyn exsl saxon user date">
	
	<xsl:output method="xml" indent="yes"/>
	<xsl:strip-space elements="*"/>
	<xsl:template match="/">
		<dexter:root>
			<day>
				<dexter:groups>
					<xsl:for-each select=".//b/h4">
						<dexter:group>
							<title><xsl:value-of select="key('hafter', last() - position())"/></title>
							<entries>
								<dexter:groups>
									<xsl:for-each select="key('pafter', last() - position())">
										<xsl:if test="position() &lt; 5">
											<dexter:group><xsl:value-of select="."/></dexter:group>
										</xsl:if>
									</xsl:for-each>
								</dexter:groups>
							</entries>
							
							<nest>
								<dexter:groups>
									<xsl:for-each select="//b">
										<xsl:if test="position() &lt; 5">
											<dexter:group><xsl:value-of select="."/></dexter:group>
										</xsl:if>
									</xsl:for-each>
								</dexter:groups>
							</nest>
							
						</dexter:group>
					</xsl:for-each>
				</dexter:groups>
			</day>
		</dexter:root>
	</xsl:template>
	
	
	<xsl:key name="hafter" match="//b/h4" use="count(following::b/h4)" />
	<xsl:key name="pafter" match="//p/a" use="count(following::b/h4)" />
	<xsl:key name="bafter" match="//b" use="count(following::b/h4)" />
</xsl:stylesheet>





