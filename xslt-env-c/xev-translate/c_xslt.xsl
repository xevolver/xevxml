<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:exslt="http://exslt.org/common" >

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
			
			<!--	#pragma xev scalar2array1_varref start(スカラ変数,添字)
			
				変数の参照をスカラ変数から1次元配列に置き換える
				
				変数の変換は指定範囲内を行う
					#pragma xev scalar2array1_varref start
				 	      ～この間変数を変換する～
					#pragma xev end scalar2array1_varref
			-->
			<xsl:when test="./@name=//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='scalar2array1_varref']/CLAUSE[@name='start']/ARG[1]/@value">
				<xsl:choose>
					<!-- 
						現在ノードより文書順で前にある最も近いノードが、
						範囲指定終了【#pragma xev end scalar2array1_varref(スカラ変数,サイズ,添字)】の場合
						変換処理を行わない【範囲外】
					-->
					<xsl:when test="preceding::DIRECTIVE[./CLAUSE/ARG[1]/@value=current()/@name and @name='end' or @name='scalar2array1_varref'][1]/CLAUSE/@name='scalar2array1_varref'">
						<xsl:copy>
							<xsl:copy-of select="@*"/>
							<xsl:apply-templates/>
						</xsl:copy>
					</xsl:when>

					<!--
						変換の有効範囲内の場合、スカラ変数名＋"_tmp" の１次元変数として参照する
					-->
					<xsl:otherwise>
						<xsl:element name="SgPntrArrRefExp">
							<!-- １次元変数名を設定する -->
							<xsl:element name="SgVarRefExp">
								<xsl:attribute name="name">
									<xsl:value-of select="concat(./@name,'_tmp')"/>
								</xsl:attribute>
							</xsl:element>

							<!-- 配列の添字を設定する -->
							<!--
							<xsl:element name="SgExprListExp">
							-->
								<xsl:element name="SgVarRefExp">
									<xsl:attribute name="name">
										<xsl:value-of select="preceding::DIRECTIVE[ @name='scalar2array1_varref' and ./CLAUSE/@name='start' and ./CLAUSE/ARG[1]/@value=current()/@name ][1]/CLAUSE/ARG[3]/@value"/>
									</xsl:attribute>
								</xsl:element>
							<!--
							</xsl:element>
							-->
		

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


	<!-- SgPntrArrRefExp -->
	<xsl:template match="SgPntrArrRefExp">
		<xsl:choose>
			<!--	#pragma xev array1to2_varref start(変数名,添字,添字)
			
				1次元配列の参照をから2次元配列に置き換える

				変数の変換は指定範囲内を行う
					#pragma xev array1to2_varref start
				 	      ～この間変数を変換する～
					#pragma xev end array1to2_varref
			-->
			<xsl:when test="./SgVarRefExp[1]/@name=preceding::DIRECTIVE[@name='array1to2_varref']/CLAUSE[@name='start']/ARG[1]/@value">
				<xsl:choose>
					<!-- 
						現在ノードより文書順で前にある最も近いノードが、
						範囲指定終了【#pragma xev end array1to2_varref(変数名,添字,添字)】の場合
						ディフォルト値を設定する
					-->
					<xsl:when test="preceding::DIRECTIVE[./CLAUSE/ARG[1]/@value=current()/SgVarRefExp[1]/@name and @name='end' or @name='array1to2_varref'][1]/CLAUSE/@name='array1to2_varref'">
						<xsl:choose>
							<!--
								ディフォルト値の指定【#pragma xev array1to2_varref start(変数名,添字,添字,default)】がある場合、ディフォルト値を設定する
							-->
							<xsl:when test="//DIRECTIVE[ @name='array1to2_varref' and ./CLAUSE/@name='start' and ./CLAUSE/ARG[1]/@value=current()/SgVarRefExp[1]/@name]/CLAUSE/ARG[4]/@value='default'">
								<xsl:copy>
									<SgPntrArrRefExp>
										<!-- 配列名を設定 -->
										<SgVarRefExp>
											<xsl:attribute name="name">
												<xsl:value-of select="//DIRECTIVE[ @name='array1to2_varref' and ./CLAUSE/@name='start' and ./CLAUSE/ARG[1]/@value=current()/SgVarRefExp[1]/@name and ./CLAUSE/ARG[4]/@value='default']/CLAUSE/ARG[1]/@value"/>
											</xsl:attribute>
										</SgVarRefExp>
										<!-- 配列の１次元目の添字を設定 -->
										<SgVarRefExp>
											<xsl:attribute name="name">
												<xsl:value-of select="//DIRECTIVE[ @name='array1to2_varref' and ./CLAUSE/@name='start' and ./CLAUSE/ARG[1]/@value=current()/SgVarRefExp[1]/@name and ./CLAUSE/ARG[4]/@value='default']/CLAUSE/ARG[2]/@value"/>
											</xsl:attribute>
										</SgVarRefExp>
									</SgPntrArrRefExp>
									<!-- 配列の２次元目の添字を設定 -->
									<SgVarRefExp>
										<xsl:attribute name="name">
											<xsl:value-of select="//DIRECTIVE[ @name='array1to2_varref' and ./CLAUSE/@name='start' and ./CLAUSE/ARG[1]/@value=current()/SgVarRefExp[1]/@name and ./CLAUSE/ARG[4]/@value='default']/CLAUSE/ARG[3]/@value"/>
										</xsl:attribute>
									</SgVarRefExp>
								</xsl:copy>

							</xsl:when>

							<!--
								ディフォルト値の指定がない場合 変換しないでそのまま設定する
							-->
							<xsl:otherwise>
								<xsl:copy>
									<xsl:copy-of select="@*"/>
									<xsl:apply-templates/>
								</xsl:copy>
							</xsl:otherwise>
						</xsl:choose>
					</xsl:when>

					<!-- 
						現在ノードより文書順で前にある【#pragma xev array1to2_varref start(変数名,添字,添字)】の
						添字設定する
					-->
					<xsl:otherwise>
						<xsl:copy>
							<SgPntrArrRefExp>
								<!-- 配列名を設定 -->
								<SgVarRefExp>
									<xsl:attribute name="name">
										<xsl:value-of select="preceding::DIRECTIVE[ @name='array1to2_varref' and ./CLAUSE/@name='start' and ./CLAUSE/ARG[1]/@value=current()/SgVarRefExp[1]/@name][1]/CLAUSE/ARG[1]/@value"/>
									</xsl:attribute>
								</SgVarRefExp>
								<!-- 配列の１次元目の添字を設定 -->
								<SgVarRefExp>
									<xsl:attribute name="name">
										<xsl:value-of select="preceding::DIRECTIVE[ @name='array1to2_varref' and ./CLAUSE/@name='start' and ./CLAUSE/ARG[1]/@value=current()/SgVarRefExp[1]/@name][1]/CLAUSE/ARG[2]/@value"/>
									</xsl:attribute>
								</SgVarRefExp>
							</SgPntrArrRefExp>
							<!-- 配列の２次元目の添字を設定 -->
							<SgVarRefExp>
								<xsl:attribute name="name">
									<xsl:value-of select="preceding::DIRECTIVE[ @name='array1to2_varref' and ./CLAUSE/@name='start' and ./CLAUSE/ARG[1]/@value=current()/SgVarRefExp[1]/@name][1]/CLAUSE/ARG[3]/@value"/>
								</xsl:attribute>
							</SgVarRefExp>
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

	<!-- SgVariableDeclaration -->
	<xsl:template match="SgVariableDeclaration">
		<xsl:choose>
			<!--	#pragma xev scalar2array1_varref start(スカラ変数,サイズ,添字)
			
				スカラ変数のワーク用１次元配列を追加する
					Ex.  INTEGER hoge　→　INTEGER hoge,hoge_tmp(サイズ)
			-->
			<!-- 指定スカラ変数の宣言の場合 -->
			<xsl:when test="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='scalar2array1_varref']/CLAUSE[@name='start']/ARG[1]/@value=./SgInitializedName/@name">
				<!-- 既存の変数宣言を出力する -->
				<xsl:copy-of select="."/>
				<!-- 既存の変数名＋"_tmp" の１次元配列宣言を行う -->
				<xsl:copy>
					<xsl:copy-of select="@*"/>

					<SgInitializedName>
						<!-- １次元配列名 -->
						<xsl:attribute name="name">
							<xsl:value-of select="concat(./SgInitializedName/@name,'_tmp')"/>
						</xsl:attribute>
						<!-- １次元配列名 -->
						<SgArrayType>
							<xsl:attribute name="index">
								<xsl:value-of select="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='scalar2array1_varref']/CLAUSE[@name='start']/ARG[1][@value=current()/SgInitializedName/@name]/following-sibling::*[1]/@value"/>
							</xsl:attribute>
							<xsl:attribute name="rank">0</xsl:attribute>
							<xsl:attribute name="type">
							<xsl:value-of select="local-name(./SgInitializedName/*[1])"/>
							</xsl:attribute>
						<xsl:copy-of select="./SgInitializedName/*[1]"/>
						</SgArrayType>
					</SgInitializedName>
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

	<!-- SgInitializedName -->
	<xsl:template match="SgInitializedName">
		<xsl:choose>


			<!--	#pragma xev array1to2 type(変数名,サイズ,サイズ)
			
				1次元配列の宣言を2次元配列に置き換える
			-->
			<xsl:when test="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='array1to2']/CLAUSE[@name='type' and @specified='true']/ARG[1]/@value=./@name">
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<SgArrayType>
						<xsl:attribute name="index">
							<xsl:value-of select="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='array1to2']/CLAUSE[@name='type' and @specified='true']/ARG[2]/@value"/>
						</xsl:attribute>
						<xsl:attribute name="rank">0</xsl:attribute>
						<xsl:attribute name="type">
							<xsl:value-of select="./SgArrayType/@type"/>
						</xsl:attribute>
					
						<SgArrayType>
							<xsl:attribute name="index">
								<xsl:value-of select="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='array1to2']/CLAUSE[@name='type' and @specified='true']/ARG[3]/@value"/>
							</xsl:attribute>
							<xsl:attribute name="rank">0</xsl:attribute>
							<xsl:attribute name="type">
								<xsl:value-of select="./SgArrayType/@type"/>
							</xsl:attribute>
						
							<xsl:copy-of select="./SgArrayType/*[1]"/>
						</SgArrayType>
						<xsl:copy-of select="./SgArrayType/*[1]"/>

					</SgArrayType>
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
			<!--	#pragma xev statement remove

				直前に'statement remove'がある場合、この statement を削除する
			-->
			<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='statement']/CLAUSE[@name='remove' and @specified='true']">
				<xsl:comment>
					remove statement
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

	
	<!-- SgWhileStmt -->
	<xsl:template match="SgWhileStmt">
		<xsl:choose>
			<!--	#pragma xev while2for replace(変数,初期値,最終値)

				直前に'while2for replace'がある場合、この【WHILE文】を【for文】に置き換える
			-->
			<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='while2for']/CLAUSE[@name='replace']">
				<SgForStatement>
					<SgForInitStatement>
						<SgExprStatement>
							<SgAssignOp>
								<SgVarRefExp>
									<xsl:attribute name="name">
										<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='while2for']/CLAUSE[@name='replace']/ARG[1]/@value"/>
									</xsl:attribute>
								</SgVarRefExp>
								<SgIntVal>
									<xsl:attribute name="value">
										<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='while2for']/CLAUSE[@name='replace']/ARG[2]/@value"/>
									</xsl:attribute>
								</SgIntVal>
							</SgAssignOp>
						</SgExprStatement>
					</SgForInitStatement>

					<SgExprStatement>
						<SgLessThanOp>
							<SgVarRefExp>
								<xsl:attribute name="name">
									<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='while2for']/CLAUSE[@name='replace']/ARG[1]/@value"/>
								</xsl:attribute>
							</SgVarRefExp>
							<SgVarRefExp>
								<xsl:attribute name="name">
									<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='while2for']/CLAUSE[@name='replace']/ARG[3]/@value"/>
								</xsl:attribute>
							</SgVarRefExp>
						</SgLessThanOp>
					</SgExprStatement>

					<!-- 刻み幅はインクリメント固定【Ex.i++】 -->
					<SgPlusPlusOp mode="1">
						<SgVarRefExp>
							<xsl:attribute name="name">
								<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='while2for']/CLAUSE[@name='replace']/ARG[1]/@value"/>
							</xsl:attribute>
						</SgVarRefExp>
					</SgPlusPlusOp>


					<!-- 【WHILE文】の処理内容【SgBasicBlock】をテンプレートを使用して複写する -->
					<xsl:apply-templates select="./SgBasicBlock"/>

				</SgForStatement>
			</xsl:when>

			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>




	<!-- SgPragmaDeclaration -->
	<xsl:template match="SgPragmaDeclaration">
		<xsl:choose>

			<!--	#pragma xev scalar2array1_varref start(変数名,サイズ,添字)

				追加した１次元配列(xxx_tmp)をfor文を使って初期化する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='scalar2array1_varref']/CLAUSE[@name='start']">
				<SgForStatement>
					<!-- 初期値を設定 -->
					<SgForInitStatement>
						<SgExprStatement>
							<SgAssignOp>
								<SgVarRefExp>
									<xsl:attribute name="name">
										<xsl:value-of select="./SgPragma/DIRECTIVE[@name='scalar2array1_varref']/CLAUSE[@name='start']/ARG[3]/@value"/>
									</xsl:attribute>
								</SgVarRefExp>
								<SgIntVal value="0"/>
							</SgAssignOp>
						</SgExprStatement>
					</SgForInitStatement>

					<!-- ループの処理条件を設定 -->
					<SgExprStatement>
						<SgLessThanOp>
							<SgVarRefExp>
								<xsl:attribute name="name">
									<xsl:value-of select="./SgPragma/DIRECTIVE[@name='scalar2array1_varref']/CLAUSE[@name='start']/ARG[3]/@value"/>
								</xsl:attribute>
							</SgVarRefExp>
							<SgVarRefExp>
								<xsl:attribute name="name">
									<xsl:value-of select="./SgPragma/DIRECTIVE[@name='scalar2array1_varref']/CLAUSE[@name='start']/ARG[2]/@value"/>
								</xsl:attribute>
							</SgVarRefExp>
						</SgLessThanOp>
					</SgExprStatement>

					<!-- ループの刻み幅を設定（インクリメント固定） -->
					<SgPlusPlusOp mode="1">
						<SgVarRefExp>
							<xsl:attribute name="name">
								<xsl:value-of select="./SgPragma/DIRECTIVE[@name='scalar2array1_varref']/CLAUSE[@name='start']/ARG[3]/@value"/>
							</xsl:attribute>
						</SgVarRefExp>
					</SgPlusPlusOp>

					<SgBasicBlock>
						<SgExprStatement>
							<SgAssignOp>
								<SgPntrArrRefExp>
									<SgVarRefExp>
										<xsl:attribute name="name">
											<xsl:value-of select="concat(./SgPragma/DIRECTIVE[@name='scalar2array1_varref']/CLAUSE[@name='start']/ARG[1]/@value,'_tmp')"/>
										</xsl:attribute>
									</SgVarRefExp>
									<SgExprListExp>
										<SgVarRefExp>
											<xsl:attribute name="name">
												<xsl:value-of select="./SgPragma/DIRECTIVE[@name='scalar2array1_varref']/CLAUSE[@name='start']/ARG[3]/@value"/>
											</xsl:attribute>
										</SgVarRefExp>
									</SgExprListExp>
								</SgPntrArrRefExp>
								<SgIntVal value="0"/>
							</SgAssignOp>
						</SgExprStatement>
					</SgBasicBlock>
				</SgForStatement>
			</xsl:when>

			<!--	#pragma xev array1to2 varref(変数名,添字,添字)

				１次元配列を２次元配列に置き替える
			-->

			<xsl:when test="./SgPragma/DIRECTIVE[@name='array1to2_varref']/CLAUSE[@name='start' and @specified='true']">
				<xsl:comment>
					SgPragmaDeclaration array1to2 varref
				</xsl:comment>
			</xsl:when>

			<!--	#pragma xev array1to2 type(変数名,サイズ,サイズ)

				１次元配列を２次元配列に置き替える
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='array1to2']/CLAUSE[@name='type' and @specified='true']">
				<xsl:comment>
					SgPragmaDeclaration array1to2 type
				</xsl:comment>
			</xsl:when>

			<!--	#pragma xev dir add

				ディレクティブを追加する	
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='add' and @specified='true']">
				<xsl:element name="SgPragmaDeclaration">
					<xsl:element name="SgPragma">
						<xsl:attribute name="pragma">
							<xsl:for-each select="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='add']/VARARG/ARG">
								<xsl:value-of select="@value" /><xsl:text> </xsl:text>
							</xsl:for-each>
						</xsl:attribute>
					</xsl:element>
				</xsl:element>
			</xsl:when>

			<!--	#pragma xev dir replace

				ディレクティブを置換する	
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='replace' and @specified='true']">
				<xsl:element name="SgPragmaDeclaration">
					<xsl:element name="SgPragma">
						<xsl:attribute name="pragma">
							<xsl:for-each select="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='replace']/VARARG/ARG">
								<xsl:value-of select="@value" /><xsl:text> </xsl:text>
							</xsl:for-each>
						</xsl:attribute>
					</xsl:element>
				</xsl:element>
			</xsl:when>

			<!--	#pragma xev dir remove

                                ディレクティブを削除する
                        -->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='remove' and @specified='true']">
				<xsl:comment>
					SgPragmaDeclaration dir remove
				</xsl:comment>
			</xsl:when>


			<!--	#pragma xev dir append( [文字列] )

                                ディレクティブ行に[文字列]を追加する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='append'and @specified='true']">
				<xsl:element name="SgPragmaDeclaration">
					<xsl:element name="SgPragma">
						<xsl:attribute name="pragma">
							<!--
								追加するディレクティブ行の内容を取り出す
								直下の【SgPragma]】を取り出す
							-->
							<xsl:value-of select="following-sibling::*[1]/SgPragma/@pragma"/>

							<!--
								接続時の半角スペースを挿入
							-->
							<xsl:text> </xsl:text>

							<!-- 
								追加する文字列を取り出
							-->
							<xsl:for-each select="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='append']/VARARG/ARG">
								<xsl:value-of select="@value" /><xsl:text> </xsl:text>
							</xsl:for-each>
						</xsl:attribute>
					</xsl:element>
				</xsl:element>
			</xsl:when>


			<!--	#pragma xev statement add( )

				指定文字列をXMLに出力する
			-->
                        <xsl:when test="./SgPragma/DIRECTIVE[@name='statement']/CLAUSE[@name='add' and @specified='true']">
				<xsl:for-each select="./SgPragma/DIRECTIVE[@name='statement']/CLAUSE[@name='add']/VARARG/ARG">
					<xsl:value-of select="@value" /><xsl:text> </xsl:text>
				</xsl:for-each>
			</xsl:when>



			<!--	#pragma xev statement remove
                                ディレクティブを削除する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='statement']/CLAUSE[@name='remove' and @specified='true']">
				<xsl:comment>
					SgPragmaDeclaration statement remove
				</xsl:comment>
			</xsl:when>


			<!--	#pragma xev while2for replace
                                ディレクティブを削除する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='while2for']/CLAUSE[@name='replace' and @specified='true']">
				<xsl:comment>
					SgPragmaDeclaration while2for replace
				</xsl:comment>
			</xsl:when>

			<!--	#pragma xev end array1to2_varref(変数名,添字,添字)
                                ディレクティブを削除する
			-->

			<xsl:when test="./SgPragma/DIRECTIVE/@name='end'">
				<xsl:comment>
					SgPragmaDeclaration end 
				</xsl:comment>
			</xsl:when>



			<xsl:otherwise>
				<xsl:choose>
					<!--直前に'xev dir appen'がある場合、この行を削除する-->
					<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='append' and @specified='true']">
						<xsl:comment>
							SgPragmaDeclaration remove
						</xsl:comment>
					</xsl:when>
					<!--直前に'xev dir replace'がある場合、この行を削除する -->
					<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='replace' and @specified='true']">
						<xsl:comment>
							SgPragmaDeclaration remove
						</xsl:comment>
					</xsl:when>
					<!--直前に'xev dir remove'がある場合、この行を削除する -->
					<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='remove' and @specified='true']">
						<xsl:comment>
							SgPragmaDeclaration remove
						</xsl:comment>
					</xsl:when>
					<xsl:otherwise>
						<xsl:copy>
							<xsl:copy-of select="@*"/>
							<xsl:apply-templates/>
						</xsl:copy>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


	<!-- PreprocessingInfo -->
	<xsl:template match="PreprocessingInfo">
		<xsl:choose>
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
