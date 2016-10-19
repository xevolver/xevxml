<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:template match="*[parent::SgVariableDeclaration][count(preceding-sibling::*)=0]">
  </xsl:template>

  <xsl:template match="SgIfStmtWithoutElse">
    <xsl:element name="SgIfStmt">
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates select="*"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="SgFunctionParameterList/SgVarRefExp">
    <xsl:element name="SgInitializedName">
      <xsl:copy-of select="@name"/>
      <xsl:element name="SgTypeInt"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="SgAssignOp[*[1]/self::SgPntrArrRefExp][parent::SgVariableDeclaration]">
    <xsl:element name="SgInitializedName">
      <xsl:attribute name="name"><xsl:value-of select="SgPntrArrRefExp/SgVarRefExp/@name"/></xsl:attribute>
      <xsl:element name="SgAssignInitializer">
	<xsl:element name="SgAggregateInitializer">
	  <xsl:copy-of select="*[2]"/>
	  <xsl:element name="SgTypeDefault"/>
	</xsl:element>
      </xsl:element>
      <xsl:element name="SgArrayType">
	<xsl:attribute name="rank"><xsl:value-of select="count(SgPntrArrRefExp/SgExprListExp/*)"/></xsl:attribute>
	<xsl:copy-of select="../*[2]"/>
	<xsl:copy-of select="SgPntrArrRefExp/SgExprListExp"/>
	<xsl:element name="SgNullExpression"/>
      </xsl:element>
    </xsl:element>
  </xsl:template>

  <xsl:template match="SgAssignOp[SgConstructorInitializer][parent::SgVariableDeclaration]">
    <xsl:element name="SgInitializedName">
      <xsl:attribute name="name"><xsl:value-of select="SgVarRefExp/@name"/></xsl:attribute>
      <xsl:apply-templates select="SgConstructorInitializer"/>
      <xsl:apply-templates select="../*[2]"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="SgAssignOp[count(*[1]/self::SgPntrArrRefExp) + count(*[2]/self::SgConstructorInitializer)=0][parent::SgVariableDeclaration]">
    <xsl:element name="SgInitializedName">
      <xsl:attribute name="name"><xsl:value-of select="SgVarRefExp/@name"/></xsl:attribute>
      <xsl:element name="SgAssignInitializer">
	<xsl:apply-templates select="*[2]"/>
      </xsl:element>
      <xsl:apply-templates select="../*[2]"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="SgPntrArrRefExp[parent::SgVariableDeclaration][count(preceding-sibling::SgPointerType)=0]">
    <xsl:element name="SgInitializedName">
      <xsl:attribute name="name"><xsl:value-of select="SgVarRefExp/@name"/></xsl:attribute>
      <xsl:element name="SgArrayType">
	<xsl:attribute name="rank"><xsl:value-of select="count(SgExprListExp/*)"/></xsl:attribute>
	<xsl:copy-of select="../*[2]"/>
	<xsl:copy-of select="SgExprListExp"/>
	<xsl:element name="SgNullExpression"/>
      </xsl:element>
    </xsl:element>
  </xsl:template>

  <xsl:template match="SgPntrArrRefExp[parent::SgVariableDeclaration][preceding-sibling::SgPointerType]">
    <xsl:element name="SgInitializedName">
      <xsl:attribute name="name"><xsl:value-of select="SgVarRefExp/@name"/></xsl:attribute>
      <xsl:element name="SgPointerType">
	<xsl:element name="SgArrayType">
	  <xsl:attribute name="rank"><xsl:value-of select="count(SgExprListExp/*)"/></xsl:attribute>
	  <xsl:copy-of select="../*[2]"/>
	  <xsl:copy-of select="SgExprListExp"/>
	  <xsl:element name="SgNullExpression"/>
	</xsl:element>
      </xsl:element>
    </xsl:element>
  </xsl:template>

  <xsl:template match="SgVarRefExp[parent::SgVariableDeclaration]">
    <xsl:element name="SgInitializedName">
      <xsl:attribute name="name"><xsl:value-of select="self::*/@name"/></xsl:attribute>
      <xsl:copy-of select="../*[2]"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="SgInitializedName[parent::SgProcedureHeaderStatement]">
    <xsl:copy>
      <xsl:attribute name="name">
	<xsl:if test="../@result">
	  <xsl:value-of select="../@result"/>
	</xsl:if>
	<xsl:if test="count(../@result)=0">
	  <xsl:value-of select="../@name"/>
	</xsl:if>
      </xsl:attribute>
      <xsl:apply-templates select="following-sibling::*[1]"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="SgVariableDeclaration">
    <xsl:copy>
      <xsl:copy-of select="@declaration_modifier"/>
      <xsl:attribute name="type_modifier">
	<xsl:value-of select="2 + count(modifier_list/SgStringVal[@value='allocatable']) * 8 + count(modifier_list/SgStringVal[@value='asynchronous']) * 16 + count(modifier_list/SgFunctionCallExp) * 32 + count(modifier_list/SgStringVal[@value='data']) * 64 + count(modifier_list/SgStringVal[@value='intent(in)']) * 256 + count(modifier_list/SgStringVal[@value='intent(out)']) * 512 + count(modifier_list/SgStringVal[@value='intent(inout)']) * 1024 + count(modifier_list/SgStringVal[@value='intrinsic']) * 2048 + count(modifier_list/SgStringVal[@value='optional']) * 4096 + count(modifier_list/SgStringVal[@value='save']) * 32768 + count(modifier_list/SgStringVal[@value='target']) * 65536 + count(modifier_list/SgStringVal[@value='value']) * 131072 "/>
      </xsl:attribute>
      <xsl:attribute name="cv_modifier">
	<xsl:value-of select="0 + count(modifier_list/SgStringVal[@value='parameter']) * 2 + count(modifier_list/SgStringVal[@value='volatile']) * 3"/>
      </xsl:attribute>
      <xsl:attribute name="access_modifier">
	<xsl:value-of select="count(modifier_list/SgStringVal[@value='private']) * 1 + count(modifier_list/SgStringVal[@value='public']) * 3 + number(count(modifier_list/SgStringVal[@value='private']|modifier_list/SgStringVal[@value='public'])=0) * 4"/>
      </xsl:attribute>
      <xsl:attribute name="storage_modifier">
	<xsl:value-of select="0 + count(modifier_list/SgStringVal[@value='external']) * 2"/>
      </xsl:attribute>
      <xsl:if test="modifier_list/SgFunctionCallExp">
	<xsl:attribute name="link"><xsl:value-of select="modifier_list/SgFunctionCallExp/SgExprListExp/SgVarRefExp/@name"/></xsl:attribute>
	<xsl:if test="modifier_list/SgFunctionCallExp/SgExprListExp/SgActualArgumentExpression">
	  <xsl:attribute name="bind"><xsl:value-of select="modifier_list/SgFunctionCallExp/SgExprListExp/SgActualArgumentExpression/SgStringVal/@value"/></xsl:attribute>
	</xsl:if>
      </xsl:if>
      <xsl:apply-templates select="*[2]/following-sibling::*"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="SgAttributeSpecificationStatement[modifier_list/SgStringVal[@value='private' or @value='public' or @value='allocatable' or @value='dimension' or @value='external' or @value='intent(in)' or @value='intent(out)' or @value='intent(inout)' or @value='intrinsic' or @value='optional' or @value='parameter' or @value='pointer' or @value='save' or @value='target' or @value='value' or @value='volatile']]">
    <xsl:element name="SgAttributeSpecificationStatement">
      <xsl:if test="modifier_list/SgStringVal[@value='private']">
	<xsl:attribute name="kind">1</xsl:attribute>
      </xsl:if>
      <xsl:if test="modifier_list/SgStringVal[@value='public']">
	<xsl:attribute name="kind">2</xsl:attribute>
      </xsl:if>
      <xsl:if test="modifier_list/SgStringVal[@value='allocatable']">
	<xsl:attribute name="kind">3</xsl:attribute>
      </xsl:if>
      <xsl:if test="modifier_list/SgStringVal[@value='dimension']">
	<xsl:attribute name="kind">7</xsl:attribute>
      </xsl:if>
      <xsl:if test="modifier_list/SgStringVal[@value='external']">
	<xsl:attribute name="kind">8</xsl:attribute>
      </xsl:if>
      <xsl:if test="modifier_list/SgStringVal[@value='intent(in)']">
	<xsl:attribute name="kind">9</xsl:attribute>
	<xsl:attribute name="intent">600</xsl:attribute>
      </xsl:if>
      <xsl:if test="modifier_list/SgStringVal[@value='intent(out)']">
	<xsl:attribute name="kind">9</xsl:attribute>
	<xsl:attribute name="intent">601</xsl:attribute>
      </xsl:if>
      <xsl:if test="modifier_list/SgStringVal[@value='intent(inout)']">
	<xsl:attribute name="kind">9</xsl:attribute>
	<xsl:attribute name="intent">602</xsl:attribute>
      </xsl:if>
      <xsl:if test="modifier_list/SgStringVal[@value='intrinsic']">
	<xsl:attribute name="kind">10</xsl:attribute>
      </xsl:if>
      <xsl:if test="modifier_list/SgStringVal[@value='optional']">
	<xsl:attribute name="kind">11</xsl:attribute>
      </xsl:if>
      <xsl:if test="modifier_list/SgStringVal[@value='parameter']">
	<xsl:attribute name="kind">12</xsl:attribute>
      </xsl:if>
      <xsl:if test="modifier_list/SgStringVal[@value='pointer']">
	<xsl:attribute name="kind">13</xsl:attribute>
      </xsl:if>
      <xsl:if test="modifier_list/SgStringVal[@value='save']">
	<xsl:attribute name="kind">15</xsl:attribute>
      </xsl:if>
      <xsl:if test="modifier_list/SgStringVal[@value='target']">
	<xsl:attribute name="kind">16</xsl:attribute>
      </xsl:if>
      <xsl:if test="modifier_list/SgStringVal[@value='value']">
	<xsl:attribute name="kind">17</xsl:attribute>
      </xsl:if>
      <xsl:if test="modifier_list/SgStringVal[@value='volatile']">
	<xsl:attribute name="kind">18</xsl:attribute>
      </xsl:if>
      <xsl:apply-templates select="*[2]|*[2]/following-sibling::*" mode="attr"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="SgVarRefExp" mode="attr">
    <xsl:element name="SgStringVal">
      <xsl:attribute name="value"><xsl:value-of select="@name"/></xsl:attribute>
    </xsl:element>
  </xsl:template>

  <xsl:template match="node()|@*" mode="attr">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates select="node()"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="SgAttributeSpecificationStatement[modifier_list/SgStringVal[@value='allocatable' or @value='dimension' or @value='parameter']]">
    <xsl:element name="SgAttributeSpecificationStatement">
      <xsl:if test="modifier_list/SgStringVal[@value='allocatable']">
	<xsl:attribute name="kind">3</xsl:attribute>
      </xsl:if>
      <xsl:if test="modifier_list/SgStringVal[@value='dimension']">
	<xsl:attribute name="kind">7</xsl:attribute>
      </xsl:if>
      <xsl:if test="modifier_list/SgStringVal[@value='parameter']">
	<xsl:attribute name="kind">12</xsl:attribute>
      </xsl:if>
      <xsl:copy-of select="*[2]|*[2]/following-sibling::*"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="SgAttributeSpecificationStatement[modifier_list/SgStringVal[@value='external']]">
    <xsl:element name="SgAttributeSpecificationStatement">
      <xsl:if test="modifier_list/SgStringVal[@value='external']">
	<xsl:attribute name="kind">8</xsl:attribute>
      </xsl:if>
      <xsl:apply-templates select="*[2]|*[2]/following-sibling::*" mode="external"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="SgVarRefExp" mode="external">
    <xsl:element name="SgFunctionRefExp">
      <xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
    </xsl:element>
  </xsl:template>

  <xsl:template match="node()|@*" mode="external">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates select="node()"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="SgProcedureHeaderStatement">
    <xsl:copy>
      <xsl:copy-of select="@kind"/>
      <xsl:copy-of select="@result"/>
      <xsl:copy-of select="@name"/>
      <xsl:copy-of select="@end_name"/>
      <xsl:if test="modifier_list/SgStringVal[@value='pure']">
	<xsl:attribute name="pure">1</xsl:attribute>
      </xsl:if>
      <xsl:if test="modifier_list/SgStringVal[@value='recursive']">
	<xsl:attribute name="recursive">1</xsl:attribute>
      </xsl:if>
      <xsl:if test="modifier_list/SgStringVal[@value='elemental']">
	<xsl:attribute name="elemental">1</xsl:attribute>
      </xsl:if>
      <xsl:if test="modifier_list/SgFunctionCallExp">
	<xsl:attribute name="declaration_modifier">66</xsl:attribute>
	<xsl:attribute name="link"><xsl:value-of select="modifier_list/SgFunctionCallExp/SgExprListExp/SgVarRefExp/@name"/></xsl:attribute>
	<xsl:if test="modifier_list/SgFunctionCallExp/SgExprListExp/SgActualArgumentExpression">
	  <xsl:attribute name="bind"><xsl:value-of select="modifier_list/SgFunctionCallExp/SgExprListExp/SgActualArgumentExpression/SgStringVal/@value"/></xsl:attribute>
	</xsl:if>
      </xsl:if>
      <xsl:apply-templates select="*[2]|*[2]/following-sibling::*"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="SgDataStatementGroup">
    <xsl:copy>
      <xsl:copy-of select="SgDataStatementObject"/>
      <xsl:for-each select="SgDataStatementValue/*">
	<xsl:element name="SgDataStatementValue">
	  <xsl:if test="count(SgDataStatementValue/SgMultiplyOp)=0">
	    <xsl:attribute name="fmt">2</xsl:attribute>
	    <xsl:element name="SgExprListExp">
	      <xsl:copy-of select="self::*"/>
	    </xsl:element>
	  </xsl:if>
	  <xsl:if test="SgDataStatementValue/SgMultiplyOp">
	    <xsl:attribute name="fmt">3</xsl:attribute>
	    <xsl:element name="SgExprListExp"/>
	    <xsl:copy-of select="SgDataStatementValue/SgMultiplyOp/*"/>
	  </xsl:if>
	</xsl:element>
      </xsl:for-each>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="SgUseStatement/SgPointerAssignOp">
    <xsl:element name="SgRenamePair">
      <xsl:attribute name="lname"><xsl:value-of select="*[1]/@name"/></xsl:attribute>
      <xsl:attribute name="uname"><xsl:value-of select="*[2]/@name"/></xsl:attribute>
    </xsl:element>
  </xsl:template>

  <xsl:template match="SgUseStatement/SgVarRefExp">
    <xsl:element name="SgRenamePair">
      <xsl:attribute name="lname"><xsl:value-of select="@name"/></xsl:attribute>
      <xsl:attribute name="uname"><xsl:value-of select="@name"/></xsl:attribute>
    </xsl:element>
  </xsl:template>

  <xsl:template match="SgAssignOp[parent::SgExprListExp[parent::SgFunctionCallExp]]">
    <xsl:element name="SgActualArgumentExpression">
      <xsl:attribute name="name"><xsl:value-of select="SgVarRefExp/@name"/></xsl:attribute>
      <xsl:copy-of select="*[2]"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="SgAssignOp[parent::SgExprListExp[parent::SgConstructorInitializer]]">
    <xsl:element name="SgActualArgumentExpression">
      <xsl:attribute name="name"><xsl:value-of select="SgVarRefExp/@name"/></xsl:attribute>
      <xsl:copy-of select="*[2]"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="SgIfStmt[count(parent::SgBasicBlock/parent::SgIfStmt)=0][@end=0][@then=1]">
    <xsl:element name="SgIfStmt">
      <xsl:attribute name="end">1</xsl:attribute>
      <xsl:copy-of select="@then"/>
      <xsl:apply-templates select="*"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="SgPragmaDeclaration">
    <xsl:copy>
      <xsl:attribute name="unparse">1</xsl:attribute>
      <xsl:apply-templates select="node()"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="node()|@*">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates select="node()"/>
    </xsl:copy>
  </xsl:template>
</xsl:stylesheet>
