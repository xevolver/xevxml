<?xml version="1.0" encoding="UTF-8"?>

<!-- Xev standard translation library -->

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<!-- emit/* -->
	<xsl:import href="emit/emit.xsl" />

	<!-- find/* -->
	<xsl:import href="find/find.xsl" />

	<!-- move/* -->
	<xsl:import href="move/move.xsl" />

	<!-- loop/* -->
	<xsl:import href="loop/loop.xsl" />

	<!-- var/* -->
	<xsl:import href="var/var.xsl" />


	<xsl:output method="xml" encoding="UTF-8" />

	<xsl:template match="/">
		<xsl:apply-templates mode="xevInitHook" />
	</xsl:template>

	<xsl:template match="*">
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates />
		</xsl:copy>
	</xsl:template>

	<xsl:template match="*" mode="xevInitHook">
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates mode="xevInitHook" />
		</xsl:copy>
	</xsl:template>

</xsl:stylesheet>
