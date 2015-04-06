<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:exslt="http://exslt.org/common">

	<xsl:import href="../../../lib/libXev.xsl" />

	<xsl:output method="xml" encoding="UTF-8" />

	<xsl:template match="*" mode="xevInitHook">
		<xsl:apply-templates select="." mode="xevFindDirective">
			<xsl:with-param name="directiveName" select="'xev loop_tag'" />
		</xsl:apply-templates>
	</xsl:template>

	<xsl:template match="*" mode="xevMoveHook">
		<xsl:apply-templates select="." mode="xevGoToLoop">
			<xsl:with-param name="loopName" select="'j'" />
		</xsl:apply-templates>
	</xsl:template>

	<xsl:template match="*" mode="xevTransformationHook">
		<xsl:apply-templates select="." mode="xevLoopInterchangeMod" />
	</xsl:template>

	<xsl:template match="SgFortranDo" mode="xevLoopInterchangeMod">
		<xsl:element name="SgFortranDo">
			<xsl:copy-of select="SgBasicBlock/SgIfStmt/SgBasicBlock/SgFortranDo/@*" />
			<xsl:copy-of select="SgBasicBlock/SgIfStmt/SgBasicBlock/SgFortranDo/*[1]" />
			<xsl:copy-of select="SgBasicBlock/SgIfStmt/SgBasicBlock/SgFortranDo/*[2]" />
			<xsl:copy-of select="SgBasicBlock/SgIfStmt/SgBasicBlock/SgFortranDo/*[3]" />
			<xsl:element name="SgBasicBlock">
				<xsl:copy-of select="SgBasicBlock/@*" />
				<xsl:element name="SgIfStmt">
					<xsl:copy-of select="SgBasicBlock/SgIfStmt/@*" />
					<xsl:copy-of
						select="SgBasicBlock/SgIfStmt/SgBasicBlock/SgFortranDo/SgBasicBlock/SgIfStmt/SgExprStatement" />
					<xsl:element name="SgBasicBlock">
						<xsl:copy>
							<xsl:copy-of select="@*" />
							<xsl:copy-of select="./*[1]" />
							<xsl:copy-of select="./*[2]" />
							<xsl:copy-of select="./*[3]" />
							<xsl:element name="SgBasicBlock">
								<xsl:copy-of
									select="SgBasicBlock/SgIfStmt/SgBasicBlock/SgFortranDo/SgBasicBlock/@*" />
								<xsl:element name="SgIfStmt">
									<xsl:copy-of select="SgBasicBlock/SgIfStmt/SgExprStatement" />
									<xsl:copy-of
										select="SgBasicBlock/SgIfStmt/SgBasicBlock/SgFortranDo/SgBasicBlock/SgIfStmt/SgBasicBlock" />
								</xsl:element>
							</xsl:element>
						</xsl:copy>
					</xsl:element>
				</xsl:element>
			</xsl:element>
		</xsl:element>
	</xsl:template>

</xsl:stylesheet>
	