<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" encoding="UTF-8" />

	<xsl:template match="/">
		<xsl:apply-templates />
	</xsl:template>

	<xsl:template match="*">
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates />
		</xsl:copy>
	</xsl:template>


	<!-- SgVarRefExp -->
	<xsl:template match="SgVarRefExp">
		<xsl:choose>
			
			<!--	!$xev scalar2array1 varref(スカラ変数,添字)
			
				変数の参照をスカラ変数から1次元配列に置き換える
				2014.03.10
			-->
			<xsl:when test="./@name=//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='scalar2array1']/CLAUSE[@name='varref']/LI[1]/@value">
				<xsl:choose>
					<!-- 
						現在ノードより文書順で前にある最も近いノードが、
						範囲指定終了【!$xev scalar2array1 varref(スカラ変数,添字,end)】の場合
						変換処理を行わない
					-->
					<xsl:when test="preceding::DIRECTIVE[ @name='scalar2array1' and ./CLAUSE/@name='varref' and ./CLAUSE/LI[1]/@value=current()/@name][1]/CLAUSE/LI[3]/@value='end'">
						<xsl:copy>
							<xsl:copy-of select="@*"/>
							<xsl:apply-templates/>
						</xsl:copy>
					</xsl:when>

					<xsl:otherwise>
						<xsl:element name="SgPntrArrRefExp">
							<xsl:element name="SgVarRefExp">
								<xsl:attribute name="name">
									<xsl:value-of select="concat(./@name,'_tmp')"/>
								</xsl:attribute>
							</xsl:element>

							<xsl:element name="SgExprListExp">
								<xsl:element name="SgVarRefExp">
									<xsl:attribute name="name">
										<xsl:value-of select="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='scalar2array1']/CLAUSE[@name='varref']/LI[1][@value=current()/@name]/following-sibling::*[1]/@value"/>
									</xsl:attribute>
								</xsl:element>
							</xsl:element>
		

						</xsl:element>

						<xsl:apply-templates/>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:when>

			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


	<!-- SgExprListExp -->
	<xsl:template match="SgExprListExp">
		<xsl:choose>
			<!--	!$xev array1to2 varref(変数名,添字,添字)
			
				変数の参照を1次元配列から2次元配列に置き換える
				2014.03.07
			-->

			<xsl:when test="preceding-sibling::SgVarRefExp[1]/@name=preceding::DIRECTIVE[@name='array1to2']/CLAUSE[@name='varref']/LI[1]/@value">
				<xsl:choose>
					<!-- 
						現在ノードより文書順で前にある最も近いノードが、
						範囲指定終了【!$xev array1to2 varref(変数名,添字,添字,end)】の場合
						ディフォルト値を設定する
					-->
					<xsl:when test="preceding::DIRECTIVE[ @name='array1to2' and ./CLAUSE/@name='varref' and ./CLAUSE/LI[1]/@value=current()/preceding-sibling::SgVarRefExp[1]/@name][1]/CLAUSE/LI[4]/@value='end'">
						<xsl:choose>
							<!--
								ディフォルト値の指定【!$xev array1to2 varref(変数名,添字,添字,default)】がある場合、ディフォルト値を設定する
							-->
							<xsl:when test="//DIRECTIVE[ @name='array1to2' and ./CLAUSE/@name='varref' and ./CLAUSE/LI[1]/@value=current()/preceding-sibling::SgVarRefExp[1]/@name]/CLAUSE/LI[4]/@value='default'">
								<xsl:copy>
									<xsl:element name="SgVarRefExp">
										<xsl:attribute name="name">
											<xsl:value-of select="//DIRECTIVE[ @name='array1to2' and ./CLAUSE/@name='varref' and ./CLAUSE/LI[1]/@value=current()/preceding-sibling::SgVarRefExp[1]/@name and ./CLAUSE/LI[4]/@value='default']/CLAUSE/LI[2]/@value"/>

										</xsl:attribute>
									</xsl:element>
									<xsl:element name="SgVarRefExp">
										<xsl:attribute name="name">
											<xsl:value-of select="//DIRECTIVE[ @name='array1to2' and ./CLAUSE/@name='varref' and ./CLAUSE/LI[1]/@value=current()/preceding-sibling::SgVarRefExp[1]/@name and ./CLAUSE/LI[4]/@value='default']/CLAUSE/LI[3]/@value"/>
										</xsl:attribute>
									</xsl:element>
								</xsl:copy>

							</xsl:when>

							<!--
								ディフォルト値の指定がない場合
								はじめに見つかった【!$xev array1to2 varref(変数名,添字,添字)】の添字を設定する
							-->
							<xsl:otherwise>
								<xsl:copy>
									<xsl:element name="SgVarRefExp">
										<xsl:attribute name="name">
											<xsl:value-of select="//DIRECTIVE[ @name='array1to2' and ./CLAUSE/@name='varref' and ./CLAUSE/LI[1]/@value=current()/preceding-sibling::SgVarRefExp[1]/@name]/CLAUSE/LI[2]/@value"/>

										</xsl:attribute>
									</xsl:element>
									<xsl:element name="SgVarRefExp">
										<xsl:attribute name="name">
											<xsl:value-of select="//DIRECTIVE[ @name='array1to2' and ./CLAUSE/@name='varref' and ./CLAUSE/LI[1]/@value=current()/preceding-sibling::SgVarRefExp[1]/@name]/CLAUSE/LI[3]/@value"/>
										</xsl:attribute>
									</xsl:element>
								</xsl:copy>
							</xsl:otherwise>
						</xsl:choose>
					</xsl:when>

					<!-- 
						現在ノードより文書順で前にある【!$xev array1to2 varref(変数名,添字,添字)】の
						添字設定する
					-->
					<xsl:otherwise>
						<xsl:copy>
							<xsl:element name="SgVarRefExp">
								<xsl:attribute name="name">
									<xsl:value-of select="preceding::DIRECTIVE[ @name='array1to2' and ./CLAUSE/@name='varref' and ./CLAUSE/LI[1]/@value=current()/preceding-sibling::SgVarRefExp[1]/@name][1]/CLAUSE/LI[2]/@value"/>

								</xsl:attribute>
							</xsl:element>
							<xsl:element name="SgVarRefExp">
								<xsl:attribute name="name">
									<xsl:value-of select="preceding::DIRECTIVE[ @name='array1to2' and ./CLAUSE/@name='varref' and ./CLAUSE/LI[1]/@value=current()/preceding-sibling::SgVarRefExp[1]/@name][1]/CLAUSE/LI[3]/@value"/>
								</xsl:attribute>
							</xsl:element>

						</xsl:copy>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:when>

			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<!-- SgInitializedName -->
	<xsl:template match="SgInitializedName">
		<xsl:choose>
			<!--	!$xev scalar2array1 type(スカラ変数,サイズ)
			
				スカラ変数のワーク用１次元配列を追加する
					Ex.  INTEGER hoge　→　INTEGER hoge,hoge_tmp(サイズ)
				2014.03.12
			-->
			<xsl:when test="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='scalar2array1']/CLAUSE[@name='type']/LI[1]/@value=./@name">
				<xsl:copy-of select="."/>
				<xsl:copy>
					<xsl:attribute name="name">
							<xsl:value-of select="concat(./@name,'_tmp')"/>
					</xsl:attribute>
					<xsl:element name="SgArrayType">
						<xsl:attribute name="index">""</xsl:attribute>
						<xsl:attribute name="rank">1</xsl:attribute>
						<xsl:attribute name="type">
							<xsl:value-of select="./SgArrayType/@type"/>
						</xsl:attribute>
					
						<xsl:copy-of select="./*[1]"/>

						<xsl:element name="SgVarRefExp">
							<xsl:attribute name="name">
								<xsl:value-of select="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='scalar2array1']/CLAUSE[@name='type']/LI[1][@value=current()/@name]/following-sibling::*[1]/@value"/>
							</xsl:attribute>
						</xsl:element>
					</xsl:element>
				</xsl:copy>
			</xsl:when>

			<!--	!$xev array1to2 type(変数名,サイズ,サイズ)
			
				1次元配列の宣言を2次元配列に置き換える
				2014.03.06
			-->
			<xsl:when test="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='array1to2']/CLAUSE[@name='type']/LI[1]/@value=./@name">
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:element name="SgArrayType">
						<xsl:attribute name="index">
							<xsl:value-of select="./SgArrayType/@index"/>
						</xsl:attribute>
						<xsl:attribute name="rank">2</xsl:attribute>
						<xsl:attribute name="type">
							<xsl:value-of select="./SgArrayType/@type"/>
						</xsl:attribute>
					
						<xsl:copy-of select="./SgArrayType/*[1]"/>

						<xsl:element name="SgVarRefExp">
							<xsl:attribute name="name">
								<xsl:value-of select="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='array1to2']/CLAUSE[@name='type']/LI[1][@value=current()/@name]/following-sibling::*[1]/@value"/>
							</xsl:attribute>
						</xsl:element>
						<xsl:element name="SgVarRefExp">
							<xsl:attribute name="name">
								<xsl:value-of select="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='array1to2']/CLAUSE[@name='type']/LI[1][@value=current()/@name]/following-sibling::*[2]/@value"/>
							</xsl:attribute>
						</xsl:element>
					</xsl:element>
				</xsl:copy>
			</xsl:when>


			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


	<!-- SgFunctionParameterList -->
	<xsl:template match="SgFunctionParameterList">
		<xsl:choose>
			<!--	!$xev parameter add

				親ノードの直近の兄ノードが【!$xev parameter add】ならパラメータを追加する
			-->
			<xsl:when test="parent::node()/preceding-sibling::SgPragmaDeclaration[1]/SgPragma/DIRECTIVE[@name='parameter']/CLAUSE[@name='add']">
				<xsl:copy>
					<!-- 既存のパラメータを出力する -->
					<xsl:copy-of select="./SgInitializedName"></xsl:copy-of>
					<!-- 追加するパラメータを出力する -->
					<xsl:element name="SgInitializedName">
						<xsl:attribute name="name">
							<xsl:value-of select="parent::node()/preceding-sibling::SgPragmaDeclaration[1]/SgPragma/DIRECTIVE[@name='parameter']/CLAUSE[@name='add']/LI/@value"/>
						</xsl:attribute>
						<xsl:element name="SgTypeFloat"/>
					</xsl:element>
				</xsl:copy>
			</xsl:when>

			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>

	</xsl:template>


	<!-- SgVariableDeclaration -->
	<xsl:template match="SgVariableDeclaration">
		<xsl:choose>
			<!--	!$xev parameter add

				子ノードに【!$xev parameter add】が存在する場合、パラメータを追加する
			-->
			<xsl:when test="./SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='parameter']/CLAUSE[@name='add']" >
				<xsl:copy>
					<!-- 既存のパラメータを出力する -->
					<xsl:copy-of select="./SgInitializedName"></xsl:copy-of>
					<!-- 追加するパラメータを出力する -->
					<xsl:element name="SgInitializedName">
						<xsl:attribute name="name">
							<xsl:value-of select="./SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='parameter']/CLAUSE[@name='add']/LI/@value"/>
						</xsl:attribute>
						<xsl:element name="SgTypeFloat"/>
					</xsl:element>
				</xsl:copy>
			</xsl:when>

			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


	<!-- SgExprStatement -->
	<xsl:template match="SgExprStatement">
		<xsl:choose>
			<!--	!$xev statement-del ptn-001

				直前に'statement-del'がある場合、この statement を削除する
			-->
			<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='statement-del']/CLAUSE[@name='ptn-001']">
				<xsl:comment>
					remove statement
				</xsl:comment>
			</xsl:when>

			<!--	!$xev statement-rep ptn-001

				直前に'statement-rep'がある場合、この statement を置き換える
			-->
			<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='statement-rep']/CLAUSE[@name='ptn-001']">
				<xsl:comment>
					remove statement
				</xsl:comment>
                        	<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='statement-rep']/CLAUSE[@name='ptn-001']/LI[1]/@value" />
                        	<xsl:text>,</xsl:text>
                                <xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='statement-rep']/CLAUSE[@name='ptn-001']/LI[2]/@value" />
                                <xsl:text>)=B(i,j)</xsl:text>
			</xsl:when>

			<!--	!$xev function-call copy-001(zz) 

				直前に'function-call copy-001'がある場合、この関数呼び出しをコピーし
				指定したパラメータに書き換える			     　
			-->
			<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='function-call']/CLAUSE[@name='copy-001']">
				<!-- 既存の【function call】をそのままコピーする -->
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>

				<!-- 追加の【function call】を出力する -->
				<xsl:copy>
					<SgFunctionCallExp>
						<!-- 関数名をそのままコピーする -->
						<xsl:copy-of select="./SgFunctionCallExp/SgFunctionRefExp"/>
						<!-- 関数の引数を、指定内容で作成する -->
						<SgExprListExp>
							<xsl:for-each select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='function-call']/CLAUSE[@name='copy-001']/LI">
 								<SgVarRefExp>
									<xsl:attribute name="name">
										<xsl:value-of select="@value" />
									</xsl:attribute>
								</SgVarRefExp>
							</xsl:for-each>
						</SgExprListExp>
					</SgFunctionCallExp>
				</xsl:copy>
			</xsl:when>

			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	
	<!-- SgWhileStmt -->
	<xsl:template match="SgWhileStmt">
		<xsl:choose>
			<!--	!$xev while2do replace(変数,初期値,最終値[,刻み幅])

				直前に'while2do replace'がある場合、この【WHILE文】を【do文】に置き換える
			-->
			<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='while2do']/CLAUSE[@name='replace']">
				<xsl:element name="SgFortranDo">
					<!-- 【WHILE文】の属性をそのまま使用する -->
					<xsl:copy-of select="@*"/>

					<!-- 【変数=初期値】 -->
					<xsl:element name="SgAssignOp">
						<xsl:element name="SgVarRefExp">
							<xsl:attribute name="name">
								<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='while2do']/CLAUSE[@name='replace']/LI[1]/@value"/>
							</xsl:attribute>
						</xsl:element>
						<xsl:element name="SgIntVal">
							<xsl:attribute name="value">
								<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='while2do']/CLAUSE[@name='replace']/LI[2]/@value"/>
							</xsl:attribute>
						</xsl:element>
					</xsl:element>

					<!-- 【最終値】 -->
					<xsl:element name="SgVarRefExp">
						<xsl:attribute name="name">
							<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='while2do']/CLAUSE[@name='replace']/LI[3]/@value"/>
						</xsl:attribute>
					</xsl:element>

					<!-- 【刻み幅】 -->
					<xsl:choose>
						<!-- 【刻み幅】の指定があるとき -->
						<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='while2do']/CLAUSE[@name='replace']/LI[4]/@value">
							<xsl:element name="SgIntVal">
								<xsl:attribute name="value">
									<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='while2do']/CLAUSE[@name='replace']/LI[4]/@value"/>
								</xsl:attribute>
							</xsl:element>
						</xsl:when>

						<!-- 【刻み幅】の指定がないとき、ディフォルト値(1)を設定する -->
						<xsl:otherwise>
							<SgNullExpression/>
						</xsl:otherwise>
					</xsl:choose>

					<!-- 
					【WHILE文】の処理内容【SgBasicBlock】を変換しないで複写する
                        		<xsl:copy-of select="./SgBasicBlock" />
                        		-->
					<!-- 
					【WHILE文】の処理内容【SgBasicBlock】をテンプレートを使用して複写する
                        		-->
					<xsl:apply-templates select="./SgBasicBlock"/>

				</xsl:element>
			</xsl:when>

			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


	<!-- SgExprStatement -->
	<!-- SgFortranDo -->
	<xsl:template match="SgFortranDo">
		<xsl:choose>
			<!--	!$xev fortran-do copy-001(a(i,j) = b(i,j))

				直前に'fortran-do copy-001'がある場合、この【DO文】をコピーし、
				処理部に【a(i,j) = b(i,j)】を設定する
			-->
			<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='fortran-do']/CLAUSE[@name='copy-001']">
				<!-- 既存の【SgFortranDo】をそのままコピーする -->
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>

				<!--
					【SgFortranDo】の処理内容を【a(i,j) = b(i,j)】に変更する
					mode="fortran-do-copy-001"のテンプレートを使用してコピーする
				-->
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates select="./*" mode="fortran-do-copy-001"/>
				</xsl:copy>

			</xsl:when>

			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>



	<!-- SgFortranDoをコピーして、処理に【a(i,j) = b(i,j)】を設定する【ここから】 -->
	<!-- SgExprStatement -->
	<xsl:template match="SgExprStatement" mode="fortran-do-copy-001">
		<xsl:copy>
			<SgAssignOp>
				<SgPntrArrRefExp>
					<SgVarRefExp name="a"/>
					<SgExprListExp>
						<SgVarRefExp name="i"/>
						<SgVarRefExp name="j"/>
					</SgExprListExp>
				</SgPntrArrRefExp>
				<SgPntrArrRefExp>
					<SgVarRefExp name="b"/>
					<SgExprListExp>
						<SgVarRefExp name="i"/>
						<SgVarRefExp name="j"/>
					</SgExprListExp>
				</SgPntrArrRefExp>
			</SgAssignOp>
		</xsl:copy>
	</xsl:template>
	<xsl:template match="*" mode="fortran-do-copy-001">
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates mode="fortran-do-copy-001"/>
		</xsl:copy>
	</xsl:template>
	<!-- remove PreprocessingInfo -->
	<xsl:template match="PreprocessingInfo"  mode="fortran-do-copy-001">
	</xsl:template>
	<!-- remove SgPragmaDeclaration -->
	<xsl:template match="SgPragmaDeclaration" mode="fortran-do-copy-001">
	</xsl:template>
	<!-- SgFortranDoをコピーして、処理に【a(i,j) = b(i,j)】を設定する【ここまで】 -->



	<!-- SgPragmaDeclaration -->
	<xsl:template match="SgPragmaDeclaration">
		<xsl:choose>
			<!--	!$xev scalar2array1 type(変数名,サイズ)

				１次元配列を２次元配列に置き替える
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='scalar2array1']/CLAUSE[@name='type'] and not(contains(./SgPragma/DIRECTIVE[@name='scalar2array1']/CLAUSE[@name='type']/LI[1]/@value,'default'))">
				<xsl:comment>
					SgPragmaDeclaration
				</xsl:comment>
			</xsl:when>

			<!--	!$xev scalar2array1 varref(変数名,添字)

				１次元配列を２次元配列に置き替える
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='scalar2array1']/CLAUSE[@name='varref'] and not(contains(./SgPragma/DIRECTIVE[@name='scalar2array1']/CLAUSE[@name='varref']/LI[1]/@value,'default'))">
				<xsl:comment>
					SgPragmaDeclaration
				</xsl:comment>
			</xsl:when>
			<!--	!$xev array1to2 varref(変数名,添字,添字)

				１次元配列を２次元配列に置き替える
			-->

			<xsl:when test="./SgPragma/DIRECTIVE[@name='array1to2']/CLAUSE[@name='varref'] and not(contains(./SgPragma/DIRECTIVE[@name='array1to2']/CLAUSE[@name='varref']/LI[1]/@value,'default'))">
				<xsl:comment>
					SgPragmaDeclaration
				</xsl:comment>
			</xsl:when>

			<!--	!$xev array1to2 type(変数名,サイズ,サイズ)

				１次元配列を２次元配列に置き替える
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='array1to2']/CLAUSE[@name='type'] and not(contains(./SgPragma/DIRECTIVE[@name='array1to2']/CLAUSE[@name='type']/LI[1]/@value,'default'))">
				<xsl:comment>
					SgPragmaDeclaration
				</xsl:comment>
			</xsl:when>

			<!--	!$xev dir ,add

				ディレクティブを追加する	
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='add'] and not(contains(./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='add']/LI[1]/@value,'default'))">
				<xsl:element name="SgPragmaDeclaration">
					<xsl:element name="SgPragma">
						<xsl:attribute name="pragma">
							<xsl:for-each select="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='add']/LI">
								<xsl:value-of select="@value" /><xsl:text> </xsl:text>
							</xsl:for-each>
						</xsl:attribute>
					</xsl:element>
				</xsl:element>
			</xsl:when>

			<!--	!$xev dir replace

				ディレクティブを置換する	
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='replace'] and not(contains(./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='replace']/LI[1]/@value,'default'))">
				<xsl:element name="SgPragmaDeclaration">
					<xsl:element name="SgPragma">
						<xsl:attribute name="pragma">
							<xsl:for-each select="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='replace']/LI">
								<xsl:value-of select="@value" /><xsl:text> </xsl:text>
							</xsl:for-each>
						</xsl:attribute>
					</xsl:element>
				</xsl:element>
			</xsl:when>

			<!--	!$xev dir del(delete)

                                ディレクティブを削除する
                        -->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='del'] and not(contains(./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='del']/LI[1]/@value,'default'))">
				<xsl:comment>
					SgPragmaDeclaration
				</xsl:comment>
			</xsl:when>


			<!--	!$xev dir append( [文字列] )

                                ディレクティブ行に[文字列]を追加する

				2014.03.06
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='append'] and not(contains(./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='append']/LI[1]/@value,'default'))">
				<xsl:element name="SgPragmaDeclaration">
					<xsl:element name="SgPragma">
						<xsl:attribute name="pragma">
							<!--
								追加するディレクティブ行の内容を取り出す

								指示内容（!$xev dir append( 文字列 )と同じ【PreprocessingInfo]】ノードの
								直下の【PreprocessingInfo]】を取り出す
							-->
							<xsl:value-of select="substring( substring-after(following-sibling::*/PreprocessingInfo[contains(text(),current()/SgPragma/@pragma)]/following-sibling::*[1],'!$'), 1, string-length(substring-after(following-sibling::*/PreprocessingInfo[contains(text(),current()/SgPragma/@pragma)]/following-sibling::*[1],'!$'))-1)"/>

							<!--
								接続時の半角スペースを挿入
							-->
							<xsl:text> </xsl:text>

							<!-- 
								追加する文字列を取り出
							-->
							<xsl:for-each select="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='append']/LI">
								<xsl:value-of select="@value" /><xsl:text> </xsl:text>
							</xsl:for-each>
						</xsl:attribute>
					</xsl:element>
				</xsl:element>
			</xsl:when>

			<!--	!$xev dir append()
                                ディレクティブを削除する
			<xsl:when test="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='append']">
				<xsl:comment>
					SgPragmaDeclaration
				</xsl:comment>
			</xsl:when>
			-->


			<!--	!$xev statement-add ptn-000( )

				指定文字列をXMLに出力する
			-->
                        <xsl:when test="./SgPragma/DIRECTIVE[@name='statement-add']/CLAUSE[@name='ptn-000'] and not(contains(./SgPragma/DIRECTIVE[@name='statement-add']/CLAUSE[@name='ptn-000']/LI[1]/@value,'default'))">
				<xsl:for-each select="./SgPragma/DIRECTIVE[@name='statement-add']/CLAUSE[@name='ptn-000']/LI">
					<xsl:value-of select="@value" /><xsl:text> </xsl:text>
				</xsl:for-each>
			</xsl:when>

			<!--	!$xev statement-add ptn-001(print,*,'Found_at',maxval(found) )

				指定文字列[ print,*,'Found_at',maxval(found) ] をXMLに出力する
				
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='statement-add']/CLAUSE[@name='ptn-001'] and not(contains(./SgPragma/DIRECTIVE[@name='statement-add']/CLAUSE[@name='ptn-001']/LI[1]/@value,'default'))">
				<xsl:value-of select="./SgPragma/DIRECTIVE[@name='statement-add']/CLAUSE[@name='ptn-001']/LI[1]/@value" /><xsl:text> </xsl:text>
				<xsl:value-of select="./SgPragma/DIRECTIVE[@name='statement-add']/CLAUSE[@name='ptn-001']/LI[2]/@value" /><xsl:text>,</xsl:text>
				<xsl:value-of select="./SgPragma/DIRECTIVE[@name='statement-add']/CLAUSE[@name='ptn-001']/LI[3]/@value" /><xsl:text>,</xsl:text>
				<xsl:value-of select="./SgPragma/DIRECTIVE[@name='statement-add']/CLAUSE[@name='ptn-001']/LI[4]/@value" /><xsl:text>)</xsl:text>
			</xsl:when>

			<!--	!$xev function-call copy-001(zz)
                                ディレクティブを削除する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='function-call']/CLAUSE[@name='copy-001']">
				<xsl:comment>
					SgPragmaDeclaration
				</xsl:comment>
			</xsl:when>

			<!--	!$xev parameter add(zz)
                                ディレクティブを削除する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='parameter']/CLAUSE[@name='add']">
				<xsl:comment>
					SgPragmaDeclaration
				</xsl:comment>
			</xsl:when>

			<!--	!$xev statement-del ptn-001
                                ディレクティブを削除する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='statement-del']/CLAUSE[@name='ptn-001']">
				<xsl:comment>
					SgPragmaDeclaration
				</xsl:comment>
			</xsl:when>

			<!--	!$xev statement-rep ptn-001
                                ディレクティブを削除する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='statement-rep']/CLAUSE[@name='ptn-001']">
				<xsl:comment>
					SgPragmaDeclaration
				</xsl:comment>
			</xsl:when>

			<!--	!$xev var-type-chg ptn-001
                                ディレクティブを削除する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='var-type-chg']/CLAUSE[@name='ptn-001']">
				<xsl:comment>
					SgPragmaDeclaration
				</xsl:comment>
			</xsl:when>

			<!--	!$while2do ptn-001
                                ディレクティブを削除する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='while2do']/CLAUSE[@name='replace']">
				<xsl:comment>
					SgPragmaDeclaration
				</xsl:comment>
			</xsl:when>


			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


	<!-- PreprocessingInfo -->
	<xsl:template match="PreprocessingInfo">
		<xsl:choose>
			<!--直前に'!$xev dir append'がある場合、この行を削除する -->
			<xsl:when test="contains(preceding-sibling::PreprocessingInfo[1],'!$xev dir append')">
				<xsl:comment>
					PreprocessingInfo
				</xsl:comment>
			</xsl:when>

			<!--	!$xev dir append( [文字列] )

				直前に'!$xev dir append'がある場合
                                ディレクティブ行に[文字列]を追加する
			<xsl:when test="contains(preceding-sibling::PreprocessingInfo[1],'!$xev dir append')">
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:value-of select="concat( substring(.,1,string-length(.)-1), ' ', substring( substring-after(preceding-sibling::PreprocessingInfo[1],'('), 1, string-length(substring-after(preceding-sibling::PreprocessingInfo[1],'('))-2) )"/>
				</xsl:copy>
			</xsl:when>
			-->

			<!--'!$xev'で始まる行を削除する -->
			<xsl:when test="contains(.,'!$xev')" >
				<xsl:comment>
					PreprocessingInfo
				</xsl:comment>
			</xsl:when>

			<!--直前に'!$xev dir replace'がある場合、この行を削除する -->
			<xsl:when test="contains(preceding-sibling::PreprocessingInfo[1],'!$xev dir replace')">
				<xsl:comment>
					PreprocessingInfo
				</xsl:comment>
			</xsl:when>

			<!--直前に'!$xev dir del'がある場合、この行を削除する -->
			<xsl:when test="contains(preceding-sibling::PreprocessingInfo[1],'!$xev dir del')">
				<xsl:comment>
					PreprocessingInfo
				</xsl:comment>
			</xsl:when>

			<!-- コメント行は、そのままコピーする -->
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


</xsl:stylesheet>
