<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
 	<xsl:output method="xml" indent="yes"/>
 	<xsl:strip-space elements="*"/>

	<xsl:template match="/">
		<root>
			<name><xsl:value-of select="//h1" /></name>
			<phone><xsl:value-of select="//*[@id='bizPhone']" /></phone>
			<address><xsl:value-of select="//address" /></address>
			<reviews>
				<xsl:for-each select="//*[contains(concat( ' ', @class, ' ' ), concat( ' ', 'nonfavoriteReview', ' ' ))]">
					<group>
						<date><xsl:value-of select=".//*[contains(concat( ' ', @class, ' ' ), concat( ' ', 'ieSucks', ' ' ))]//*[contains(concat( ' ', @class, ' ' ), concat( ' ', 'smaller', ' ' ))]" /></date>
						<user-name><xsl:value-of select=".//*[contains(concat( ' ', @class, ' ' ), concat( ' ', 'reviewer_info', ' ' ))]//a" /></user-name>
						<user-link><xsl:value-of select=".//*[contains(concat( ' ', @class, ' ' ), concat( ' ', 'reviewer_info', ' ' ))]//a/@href" /></user-link>
						<comment><xsl:value-of select=".//*[contains(concat( ' ', @class, ' ' ), concat( ' ', 'review_comment', ' ' ))]" /></comment>
					</group>
				</xsl:for-each>
			</reviews>
		</root>
	</xsl:template>
</xsl:stylesheet>