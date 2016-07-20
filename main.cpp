#include <iostream>
#include <string>
#include <sys/time.h>
#include "stdlib.h"

#include "NLPIR.h"
#include "ltp/segment_dll.h"
#include "ltp/postag_dll.h"
#include "ltp/parser_dll.h"

const static std::string str_CWSModelPath = "/home/andin/soft/ltp/ltp_data/cws.model";
const static std::string str_POSModelPath = "/home/andin/soft/ltp/ltp_data/pos.model";
const static std::string str_NERModelPath = "/home/andin/soft/ltp/ltp_data/ner.model";
const static std::string str_ParserModelPath = "/home/andin/soft/ltp/ltp_data/parser.model";

/**
    分词
*/
void WordSegmentor(std::string str_Sentence,std::vector<std::string>& words)
{
    void * engine = segmentor_create_segmentor(str_CWSModelPath.c_str());
    if (!engine)
    {
        std::cout<<"Load WordSegmentor Model Error!"<<std::endl;
        exit(-1);
    }
    int len = segmentor_segment(engine,str_Sentence,words);
    segmentor_release_segmentor(engine);
}

/**
    词性标注
*/
void PartOfSpeech(std::vector<std::string> words,std::vector<std::string>& tags)
{
    void * engine = postagger_create_postagger(str_POSModelPath.c_str());
    if (!engine)
    {
        std::cout<<"Load PartOfSpeech Model Error!"<<std::endl;
        exit(-1);
    }
    postagger_postag(engine, words, tags);
    postagger_release_postagger(engine);
}

/**
    句法结构分析
*/
void Parser(std::vector<std::string> words,std::vector<std::string> postags,std::vector<int>& heads,std::vector<std::string>& deprels)
{
    void * engine = parser_create_parser(str_ParserModelPath.c_str());
    if (!engine)
    {
        std::cout<<"Load Parser Model Error!"<<std::endl;
        exit(-1);
    }
    parser_parse(engine, words, postags, heads, deprels);
    parser_release_parser(engine);
}

/**
    NLPIR分词
*/
void NLPIR(std::string str_Sentence,std::vector<std::string>& words,std::vector<std::string>& tags)
{
    //初始化分词组件
    if(!NLPIR_Init(0,UTF8_CODE,0))
    {
        std::cout<<"ICTCLAS INIT FAILED!"<<std::endl;
    }
    NLPIR_ImportUserDict("./Data/userdic.txt");
    const result_t *pVecResult;
    int nCount = 0;
    pVecResult = NLPIR_ParagraphProcessA(str_Sentence.c_str(),&nCount,true);
    for (int i=0; i<nCount; i++)
    {
        std::string str_HitsWord = str_Sentence.substr(pVecResult[i].start,pVecResult[i].length);
        words.push_back(str_HitsWord);
        tags.push_back(pVecResult[i].sPOS);
    }
    NLPIR_Exit();
}

int main()
{
    struct timeval start,finish;
    double duration;
    gettimeofday(&start,NULL);


    //std::string str_Sentence = "NLPIR系统是中科院开发的。";
    std::string str_Sentence = "国务院总理李克强调研上海外高桥时提出，支持上海积极探索新机制。";
    std::vector<std::string> words;
    std::vector<std::string> tags;

    //WordSegmentor(str_Sentence,words);
    //PartOfSpeech(words,tags);

    NLPIR(str_Sentence,words,tags);

    for (int i = 0; i < tags.size(); ++ i)
    {
        std::cout << words[i] << "/" << tags[i];
        if (i == tags.size() - 1) std::cout << std::endl;
        else std::cout << "\t";
    }

    std::vector<int> heads;
    std::vector<std::string> deprels;
    Parser(words,tags,heads,deprels);
    for (int i = 0; i < heads.size(); ++ i)
    {
        std::cout << i+1 <<": " << words[i] << "\t" << tags[i] << "\t"<< heads[i] << "\t" << deprels[i] << std::endl;
        //std::cout<<"<word id=\""<<i<<"\" cont=\"" <<words[i]<<"\" pos=\""<<tags[i]<<"\" parent=\""<<heads[i] <<"\" relate=\""<<deprels[i]<<"\"/>"<<std::endl;
    }

    gettimeofday(&finish,NULL);
    duration = 1000000 * (finish.tv_sec - start.tv_sec) + finish.tv_usec - start.tv_usec;
    duration /= 1000000;
    std::cout<<std::endl<<std::endl<<"cost "<<duration<<" s"<<std::endl<<std::endl;
    return 0;
}
