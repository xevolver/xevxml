<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:exslt="http://exslt.org/common">

	<xsl:import href="../../../lib/libCHiLL.xsl" />

	<xsl:output method="xml" encoding="UTF-8" />

	<xsl:template match="*" mode="xevInitHook">
		<xsl:apply-templates select="." />
	</xsl:template>

	<xsl:template match="SgFortranDo">
		<xsl:choose>
			<xsl:when test="preceding-sibling::*[1]/SgPragma/@pragma = 'xev loop_tag'">
				<xsl:comment>
					xev loop_tag start test-3.xsl
				</xsl:comment>

				<xsl:variable name="step1">
					<xsl:apply-templates select="." mode="chillUnrollJam">
						<xsl:with-param name="factor" select="4" />
						<xsl:with-param name="loopName" select="'k'" />
					</xsl:apply-templates>
				</xsl:variable>

				<xsl:apply-templates select="exslt:node-set($step1)"
					mode="find_loop_and_unroll" />
			</xsl:when>

			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates />
				</xsl:copy>
			</xsl:otherwise>

		</xsl:choose>
	</xsl:template>


	<xsl:template match="*" mode="find_loop_and_unroll">
		<xsl:choose>
			<xsl:when test="self::SgFortranDo/SgAssignOp/SgVarRefExp/@name = 'i'">
				<xsl:apply-templates select="." mode="chillUnroll">
					<xsl:with-param name="factor" select="2" />
					<xsl:with-param name="loopName" select="'i'" />
				</xsl:apply-templates>
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="find_loop_and_unroll" />
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


</xsl:stylesheet>
	