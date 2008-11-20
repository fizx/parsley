<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
 	<xsl:output method="xml" indent="yes"/>
 	<xsl:strip-space elements="*"/>

	<xsl:template match="/">
		<root>
			<xsl:variable name="root.name" select="//h1" />
			<name><xsl:value-of select="$root.name" /></name>
			<xsl:variable name="root.phone" select="//*[@id='bizPhone']" />
			<phone><xsl:value-of select="$root.phone" /></phone>
			<xsl:variable name="root.address" select="//address" />
			<address><xsl:value-of select="$root.address" /></address>
			<reviews>
				<xsl:for-each select="//*[contains(concat( ' ', @class, ' ' ), concat( ' ', 'nonfavoriteReview', ' ' ))]">
					<group>
						<xsl:variable name="root.reviews.date" select=".//*[contains(concat( ' ', @class, ' ' ), concat( ' ', 'ieSucks', ' ' ))]//*[contains(concat( ' ', @class, ' ' ), concat( ' ', 'smaller', ' ' ))]" />
						<date><xsl:value-of select="$root.reviews.date" /></date>
						<xsl:variable name="root.reviews.user-name" select=".//*[contains(concat( ' ', @class, ' ' ), ' reviewer_info ')]//a" />
						<user-name><xsl:value-of select="$root.reviews.user-name" /></user-name>
						<xsl:variable name="root.reviews.user-link" select="concat('http://www.yelp.com', $root.reviews.user-name / @href)" />
						<user-link><xsl:value-of select="$root.reviews.user-link" /></user-link>
						<xsl:variable name="root.reviews.comment" select=".//*[contains(concat( ' ', @class, ' ' ), ' review_comment ')]" />
						<comment><xsl:value-of select="$root.reviews.comment" /></comment>
					</group>
				</xsl:for-each>
			</reviews>
		</root>
	</xsl:template>
</xsl:stylesheet>