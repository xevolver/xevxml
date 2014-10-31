<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:exslt="http://exslt.org/common">

	<xsl:import href="../../../lib/libCHiLL.xsl" />

	<xsl:output method="xml" encoding="UTF-8" />

	<xsl:template match="SgFortranDo">
		<xsl:choose>
			<xsl:when test="preceding-sibling::*[1]/SgPragma/@pragma = 'xev loop_tag'">
				<xsl:comment>
					test-4.xsl xev loop_tag
				</xsl:comment>

				<xsl:variable name="step1">
					<xsl:apply-templates select="." mode="find_loop">
					</xsl:apply-templates>
				</xsl:variable>

				<xsl:apply-templates select="exslt:node-set($step1)"
					mode="chill_unroll_jam">
					<xsl:with-param name="max" select="4" />
					<xsl:with-param name="var" select="'k'" />
				</xsl:apply-templates>
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
				<xsl:apply-templates select="." mode="chill_unroll">
					<xsl:with-param name="max" select="4" />
					<xsl:with-param name="var" select="'i'" />
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
	