<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:import href="../../../lib/libXev.xsl" />

	<xsl:output method="xml" encoding="UTF-8" />

	<xsl:template match="*" mode="xevInitHook">
		<xsl:apply-templates select="." mode="xevFindDirective">
			<xsl:with-param name="directiveName" select="'xev loop_tag'" />
		</xsl:apply-templates>
	</xsl:template>

	<xsl:template match="*" mode="xevMoveHook">
		<xsl:apply-templates select="." mode="xevGoToLoop">
			<xsl:with-param name="loopName" select="'i'" />
		</xsl:apply-templates>
	</xsl:template>

	<xsl:template match="*" mode="xevTransformationHook">
		<xsl:apply-templates select="." mode="xevLoopUnroll">
			<xsl:with-param name="factor" select="3" />
			<xsl:with-param name="loopName" select="'i'" />
		</xsl:apply-templates>
	</xsl:template>

	<xsl:template match="SgFortranDo" mode="bkup">
		<xsl:choose>
			<xsl:when test="preceding-sibling::*[1]/SgPragma/@pragma = 'xev loop_tag'">
				<xsl:comment>
					xev loop_tag loop_unroll
				</xsl:comment>
				<xsl:apply-templates select="." mode="find_loop" />
			</xsl:when>

			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates />
				</xsl:copy>
			</xsl:otherwise>

		</xsl:choose>
	</xsl:template>

	<xsl:template match="*" mode="find_loop">
		<xsl:choose>
			<xsl:when test="self::SgFortranDo/SgAssignOp/SgVarRefExp/@name = 'i'">
				<xsl:apply-templates select="." mode="loop_unroll">
					<xsl:with-param name="factor" select="3" />
					<xsl:with-param name="loopName" select="'i'" />
				</xsl:apply-templates>
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="find_loop" />
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template match="SgPragmaDeclaration" mode="find_loop">
	</xsl:template>
	<xsl:template match="PreprocessingInfo" mode="find_loop">
	</xsl:template>

</xsl:stylesheet>
	