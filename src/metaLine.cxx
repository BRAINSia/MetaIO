#include <cstdio>
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <string.h>

#include <metaUtils.h>
#include <metaObject.h>
#include <metaLine.h>

//
// MedImage Constructors
//
MetaLine::
MetaLine()
:MetaObject()
{
  if(DEBUG) std::cout << "MetaLine()" << std::endl;
  Clear();
}

//
MetaLine::
MetaLine(const char *_headerName)
:MetaObject()
{
  if(DEBUG)  std::cout << "MetaLine()" << std::endl;
  Clear();
  Read(_headerName);
}

//
MetaLine::
MetaLine(const MetaLine *_tube)
:MetaObject()
{
  if(DEBUG)  std::cout << "MetaLine()" << std::endl;
  Clear();
  CopyInfo(_tube);
}



//
MetaLine::
MetaLine(unsigned int dim)
:MetaObject(dim)
{
  if(DEBUG) std::cout << "MetaLine()" << std::endl;
  Clear();
}

//
MetaLine::
~MetaLine()
{
  M_Destroy();
}

//
void MetaLine::
PrintInfo() const
{
  MetaObject::PrintInfo();
  std::cout << "PointDim = " << m_PointDim << std::endl;
  std::cout << "NPoints = " << m_NPoints << std::endl;
  char str[255];
  MET_TypeToString(m_ElementType, str);
  std::cout << "ElementType = " << str << std::endl;
}

void MetaLine::
CopyInfo(const MetaLine * _tube)
{
  MetaObject::CopyInfo(_tube);
}

    

void MetaLine::
PointDim(const char* pointDim)
{
  strcpy(m_PointDim,pointDim);
}
    
const char* MetaLine::
PointDim(void) const
{
  return m_PointDim;
}

void MetaLine::
NPoints(int npnt)
{
  m_NPoints = npnt;
}

int MetaLine::
NPoints(void) const
{
  return m_NPoints;
}


bool MetaLine::
ReadStream(int ndims, std::ifstream * stream)
{
  
  if(DEBUG)  std::cout << "MetaLine: ReadStream" << std::endl;

  M_Destroy();
  Clear();

  M_SetupReadFields();

  MET_FieldRecordType * mF = MET_GetFieldRecord("NDims",m_Fields);
  mF->value[0] = ndims;
  mF->defined = true;

  m_ReadStream = stream;
  bool result = M_Read();
  return result;
}

bool MetaLine::
Read(const char *_headerName)
{
  if(DEBUG) std::cout << "MetaLine: Read" << std::endl;

  M_Destroy();

  Clear();

  M_SetupReadFields();

  if(_headerName != NULL)
  {
    strcpy(m_FileName, _headerName);
  }

  if(DEBUG) std::cout << "MetaLine: Read: Opening stream" << std::endl;
 
  m_ReadStream->open(m_FileName, std::ios::binary);
  
  if(!m_ReadStream->is_open())
  {
    std::cout << "MetaLine: Read: Cannot open file" << std::endl;
    return false;
  }

  if(!M_Read())
  {
    std::cout << "MetaLine: Read: Cannot parse file" << std::endl;
    return false;
  }

  if(_headerName != NULL)
  {
    strcpy(m_FileName, _headerName);
  }

  m_ReadStream->close();

  return true;
}

//
//
//
bool MetaLine::
Write(const char *_headName, const char *_dataName)
{
  if(DEBUG) std::cout << "MetaLine: Write" << std::endl;

  if(_headName != NULL)
    {
    FileName(_headName);
    }

  m_NPoints = m_PointList.size();

  M_SetupWriteFields();

  m_WriteStream->open(m_FileName, std::ios::binary);
  if(!m_WriteStream->is_open())
    {
    return false;
    }

  bool result = M_Write();
      
  m_WriteStream->close();

  return true;
}
  

bool MetaLine
::Append(const char *_headName, const char *_dataName)
{
  if(DEBUG) std::cout << "MetaLine: Append" << std::endl;

  if(_headName != NULL)
  {
    FileName(_headName);
  }

  m_NPoints = m_PointList.size();

  M_SetupWriteFields();

  m_WriteStream->open(m_FileName, std::ios::binary | std::ios::app );
  if(!m_WriteStream->is_open())
  {
    return false;
  }

  bool result = M_Write();
      
  return true;

}




/** Clear tube information */
void MetaLine::
Clear(void)
{
  if(DEBUG) std::cout << "MetaLine: Clear" << std::endl;
  MetaObject::Clear();
  m_NPoints = 0;
  m_PointList.clear();
  strcpy(m_PointDim, "x y z v1x v1y v1z");
  m_ElementType = MET_FLOAT;
}
        
/** Destroy tube information */
void MetaLine::
M_Destroy(void)
{
  MetaObject::M_Destroy();
}

/** Set Read fields */
void MetaLine::
M_SetupReadFields(void)
{
  if(DEBUG) std::cout << "MetaLine: M_SetupReadFields" << std::endl;

  MetaObject::M_SetupReadFields();

  MET_FieldRecordType * mF;

  int nDimsRecNum = MET_GetFieldRecordNumber("NDims",m_Fields);

  mF = new MET_FieldRecordType;
  MET_InitReadField(mF, "PointDim", MET_STRING, true);
  m_Fields.push_back(mF);

  mF = new MET_FieldRecordType;
  MET_InitReadField(mF, "NPoints", MET_INT, true);
  m_Fields.push_back(mF);

  mF = new MET_FieldRecordType;
  MET_InitReadField(mF, "ElementType", MET_STRING, true);
  mF->required = true;
  m_Fields.push_back(mF);

  mF = new MET_FieldRecordType;
  MET_InitReadField(mF, "Points", MET_NONE, true);
  mF->terminateRead = true;
  m_Fields.push_back(mF);

}

void MetaLine::
M_SetupWriteFields(void)
{
  strcpy(m_ObjectTypeName,"Line");
  MetaObject::M_SetupWriteFields();

  MET_FieldRecordType * mF;

  char s[255];
  mF = new MET_FieldRecordType;
  MET_TypeToString(m_ElementType, s);
  MET_InitWriteField(mF, "ElementType", MET_STRING, strlen(s), s);
  m_Fields.push_back(mF);

  if(strlen(m_PointDim)>0)
  {
    mF = new MET_FieldRecordType;
    MET_InitWriteField(mF, "PointDim", MET_STRING,
                           strlen(m_PointDim),m_PointDim);
    m_Fields.push_back(mF);
  }
  mF = new MET_FieldRecordType;
  MET_InitWriteField(mF, "NPoints", MET_INT,m_NPoints);
  m_Fields.push_back(mF);

  mF = new MET_FieldRecordType;
  MET_InitWriteField(mF, "Points", MET_NONE);
  m_Fields.push_back(mF);

}

MET_ValueEnumType MetaLine::
ElementType(void) const
{
  return m_ElementType;
}

void MetaLine::
ElementType(MET_ValueEnumType _elementType)
{
  m_ElementType = _elementType;
}


bool MetaLine::
M_Read(void)
{
  if(DEBUG) std::cout << "MetaLine: M_Read: Loading Header" << std::endl;

  if(!MetaObject::M_Read())
  {
    std::cout << "MetaLine: M_Read: Error parsing file" << std::endl;
    return false;
  }

  if(DEBUG) std::cout << "MetaLine: M_Read: Parsing Header" << std::endl;
 
  MET_FieldRecordType * mF;
 
  mF = MET_GetFieldRecord("NPoints",m_Fields);
  if(mF->defined)
  {
    m_NPoints= (int)mF->value[0];
  }

  mF = MET_GetFieldRecord("ElementType",m_Fields);
  if(mF->defined)
  {
    MET_StringToType((char *)(mF->value), &m_ElementType);
  }

  mF = MET_GetFieldRecord("PointDim",m_Fields);
  if(mF->defined)
  {
    strcpy(m_PointDim,(char *)(mF->value));
  }

  int* posDim= new int[m_NDims];
  for(unsigned int i= 0; i < m_NDims; i++)
  {
    posDim[i] = -1;
  }

  int posV1x = -1;
  int posV1y = -1;
  int posV1z = -1;

  int pntDim;
  char** pntVal = NULL;
  MET_StringToWordArray(m_PointDim, &pntDim, &pntVal); 

  float v[16];
  LinePnt* pnt;
  
  if(m_BinaryData)
  {
    int elementSize;
    MET_SizeOfType(m_ElementType, &elementSize);
    int readSize = m_NPoints*(m_NDims*m_NDims+4)*elementSize;

    char* _data = new char[readSize];
    m_ReadStream->read((char *)_data, readSize);

    int gc = m_ReadStream->gcount();
    if(gc != readSize)
    {
      std::cout << "MetaLine: m_Read: data not read completely" 
                << std::endl;
      std::cout << "   ideal = " << readSize << " : actual = " << gc << std::endl;
      return false;
    }

    unsigned int i=0;
    double td;
    for(unsigned int j=0; j<m_NPoints; j++) 
    {
      pnt = new LinePnt(m_NDims);
      float* x = new float[m_NDims];
      
      for(unsigned int d=0; d<m_NDims; d++)
      {
        MET_ValueToDouble(m_ElementType, _data, i++, &td);
        x[d] = (float)td;
      }

      for(unsigned int l=0;l<m_NDims-1;l++)
      {
        float* n = new float[m_NDims];
        for(unsigned int d=0; d<m_NDims; d++)
        {
          MET_ValueToDouble(m_ElementType, _data, i++, &td);
          n[d] = (float)td;
        }
        pnt->m_V[l] = n; 
      }
      pnt->m_X = x; 
      m_PointList.push_back(pnt);
    }
  }
  else
  {
    for(unsigned int j=0; j<m_NPoints; j++) 
    {
      pnt = new LinePnt(m_NDims);

      unsigned int k=0;
      unsigned int d=0;
      for(k=0; k<m_NDims; k++)
      {
        *m_ReadStream >> v[k];
        char c = m_ReadStream->get();
      }

      float* x = new float[m_NDims];
      for(d=0; d<m_NDims; d++)
      {
        x[d] = v[d];
      }

      pnt->m_X = x;
      

      for(k=0; k<m_NDims-1; k++)
      {
        for(unsigned int j=0; j<m_NDims; j++)
        {
          *m_ReadStream >> v[j];
          char c = m_ReadStream->get();
        }

        float* n = new float[m_NDims];
        for(d=0; d<m_NDims; d++)
        {
          n[d] = v[d];
        }
        pnt->m_V[k] = n;
      }

      for(k=0; k<4; k++)
      {
        *m_ReadStream >> v[k];
        char c = m_ReadStream->get();
        pnt->m_Color[k] = v[k];
      }

      m_PointList.push_back(pnt);
    }

      
    char c = ' ';
    while( (c!='\n') && (!m_ReadStream->eof()))
    {
      c = m_ReadStream->get();// to avoid unrecognize charactere
    }
  }

  return true;
}


bool MetaLine::
M_Write(void)
{

  if(!MetaObject::M_Write())
  {
    std::cout << "MetaLine: M_Read: Error parsing file" << std::endl;
    return false;
  }

  /** Then copy all points */
  
  if(m_BinaryData)
  {
    PointListType::const_iterator it = m_PointList.begin();
    int elementSize;
    MET_SizeOfType(m_ElementType, &elementSize);

    char* data = new char[m_NDims*m_NPoints*elementSize*m_NDims];
    unsigned int i=0;
    unsigned int d=0;
    while(it != m_PointList.end())
    {
      for(d = 0; d < m_NDims; d++)
      {
        MET_DoubleToValue((double)(*it)->m_X[d],m_ElementType,data,i++);
        
      }

      for(unsigned int j=0;j<m_NDims-1;j++)
      {
        for(d=0; d<m_NDims; d++)
        {
          MET_DoubleToValue((double)(*it)->m_V[j][d],m_ElementType,data,i++);
        }
      }

      for(d=0; d<4; d++)
      {
        MET_DoubleToValue((double)(*it)->m_Color[d],m_ElementType,data,i++);
      }

      it++;
    }

   
    m_WriteStream->write((char *)data,(m_NDims*m_NDims+4)*m_NPoints*elementSize);  
  }
  else
  {
    PointListType::const_iterator it = m_PointList.begin();
  
    unsigned int d;
    while(it != m_PointList.end())
    {
      for(d = 0; d < m_NDims; d++)
      {
        *m_WriteStream << (*it)->m_X[d] << " ";
      }

      for(d = 0; d < m_NDims-1; d++)
      {
        for(unsigned int i = 0; i < m_NDims; i++)
        {
          *m_WriteStream << (*it)->m_V[d][i] << " ";
        }
      }

      for(d=0;d<4;d++)
      {
        *m_WriteStream << (*it)->m_Color[d] << " ";
      }

      *m_WriteStream << std::endl;
      it++;
    }
  }

  return true;

}
