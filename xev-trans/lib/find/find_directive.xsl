<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="*" mode="xevFindDirective">
		<xsl:param name="directiveName" />
		<xsl:choose>
			<xsl:when
				test="self::SgFortranDo/preceding-sibling::*[1]/SgPragma/@pragma = $directiveName">
				<xsl:apply-templates select="." mode="xevMoveHook" />
			</xsl:when>
			<xsl:when
				test="self::SgWhileStmt/preceding-sibling::*[1]/SgPragma/@pragma = $directiveName">
				<xsl:apply-templates select="." mode="xevMoveHook" />
			</xsl:when>
			<xsl:when
				test="self::SgIfStmt/preceding-sibling::*[1]/SgPragma/@pragma = $directiveName">
				<xsl:apply-templates select="." mode="xevMoveHook" />
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="xevFindDirective">
						<xsl:with-param name="directiveName" select="$directiveName" />
					</xsl:apply-templates>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template match="*" mode="xevFindNthDirective">
		<xsl:param name="directiveName" />
		<xsl:param name="N" />
		<xsl:choose>
			<xsl:when
				test="self::SgFortranDo/preceding-sibling::*[$N]/SgPragma/@pragma = $directiveName">
				<xsl:apply-templates select="." mode="xevMoveHook" />
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="xevFindDirective">
						<xsl:with-param name="directiveName" select="$directiveName" />
						<xsl:with-param name="N" select="$N" />
					</xsl:apply-templates>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


</xsl:stylesheet>
