<?xml version="1.0" encoding="shift_jis"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<!-- �֐��ďo������printf���ǂ����̃t���O��ݒ肷�� -->
  <xsl:template match="SgFunctionCallExp">
    <xsl:choose>
      <xsl:when test="./SgFunctionRefExp/@symbol='printf'">
	<xsl:copy>
	  <xsl:apply-templates>
	    <xsl:with-param name="isInPrintf" select="'yes'"/>
	  </xsl:apply-templates>
	</xsl:copy>
      </xsl:when>
      <xsl:otherwise>
	<xsl:copy>
	  <xsl:apply-templates select="node()|@*">
	    <xsl:with-param name="isInPrintf" select="'no'"/>
	  </xsl:apply-templates>
	</xsl:copy>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

<!-- �֐��ďo���̕����ؒ��ł�printf�t���O��`������ -->
  <xsl:template match="SgFunctionCallExp//*">
    <xsl:param name="isInPrintf"/>
    <xsl:copy>
      <xsl:apply-templates select="node()|@*">
	<xsl:with-param name="isInPrintf" select="$isInPrintf"/>
      </xsl:apply-templates>
    </xsl:copy>
  </xsl:template>

<!-- printf�̃t�H�[�}�b�g��������������� -->
  <xsl:template match="SgStringVal/@value">
    <xsl:param name="isInPrintf"/>
    <xsl:choose>
      <xsl:when test="$isInPrintf='yes'">
	<xsl:attribute name="value">
	  <xsl:value-of select="'Hello, Xevolver %d'"/>
	</xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
	<xsl:copy-of select="."/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

<!-- printf�̈����ɕϐ�i��ǉ� -->
  <xsl:template match="SgFunctionCallExp/SgExprListExp">
    <xsl:param name="isInPrintf"/>
    <xsl:copy>
      <xsl:apply-templates select="node()|@*">
	<xsl:with-param name="isInPrintf" select="$isInPrintf"/>
      </xsl:apply-templates>
      <xsl:if test="$isInPrintf='yes'">
	<SgVarRefExp name="i"/>
      </xsl:if>
    </xsl:copy>
  </xsl:template>


<!-- main�֐��̍ŏ���int i=2;��}��
   (�{����main�֐���body���Ɗm�F���Ă���ϐ��錾��}�����ׂ�������
    �֐���������Ȃ�����蔲��)
-->
  <xsl:template match="SgFunctionDefinition/SgBasicBlock">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
  int i = 2;
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>


<!--printf�t���O�̃f�o�b�O�p���[�� -->
<!--
  <xsl:template match="SgFunctionRefExp/@symbol">
    <xsl:param name="isInPrintf"/>
    <xsl:choose>
      <xsl:when test="$isInPrintf='yes'">
	<xsl:attribute name="symbol">
	  <xsl:value-of select="string('yes')"/>
	</xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
	<xsl:attribute name="symbol">
	  <xsl:value-of select="string('no')"/>
	</xsl:attribute>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
-->
</xsl:stylesheet>
