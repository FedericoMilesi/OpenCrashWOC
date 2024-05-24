#include "../nu.h"

//NGC MATCH
s32 SolveQuadratic(float a, float b, float c, float* t1, float* t2) {
    float x;

    if (a == 0.0f) {
        if (b == 0.0f) {
            return 0;
        }
        *t1 = *t2 = -c / b;
        return 1;
    }
    
    x = (b * b);
    if ((a * 4.0f) * c > x) {
        return 0;
    }
    
    if ((a * 4.0f) * c == x) {
        *t1 = *t2 = (-b) / (a + a);
        return 1;
    } else {
        x = NuFsqrt(x - (a * 4.0f) * c);
        *t1 = (-b - x) / (a + a);
        *t2 = (-b + x) / (a + a);
        return 1;
    }
}

//NGC MATCH
struct numtl_s* CreateCopyMat(struct numtl_s* origmtl, int uvmode, int alphamod, int zcmp, int filter) {
    struct numtl_s* mtl;

    mtl = NuMtlCreate(1);
    
    (mtl->diffuse).r = 1.0f;
    (mtl->diffuse).g = 1.0f;
    (mtl->diffuse).b = 1.0f;
    
    mtl->attrib.cull = 2;
    mtl->attrib.zmode = zcmp;
    mtl->attrib.filter = filter;
    
    mtl->alpha = 0.999f;
    
    mtl->attrib.alpha = alphamod;
    
    if (uvmode != 0) {
        mtl->attrib.uvmode = 1;
    }
    
    mtl->attrib.utc = 1;
    
    mtl->attrib.vtc = 1;
    mtl->tid = origmtl->tid;
    mtl->alpha_sort = origmtl->alpha_sort;
    mtl->attrib.afail = 1;
    mtl->attrib.colour = origmtl->attrib.colour;
    mtl->attrib.lighting = origmtl->attrib.lighting;
    NuMtlUpdate(mtl);
    return mtl;
}

//NGC MATCH
void DebrisMalloc(void) {
  if (debbuffer == NULL) {
    debbuffer = (char *)malloc_x(0x93400);
  }
  return;
}

//DONE NGC
void SetupDebris(void) {
    s32 lp;
    s32 size;
    s32 tmp;
    char* buffer;
    s32 oldSize;

    memset(debbuffer, 0, 0x93400);
    memset(freedebchunks, 0, 0x400);
    memset(freedebkeys, 0, 0x200);
    memset(debkeydata, 0, 0x56c00);
    
    DebMat[0] = CreateAlphaBlendTexture256_32("stuff\\particle.raw", 1, 2, 0x100, 0);
    DebMat[1] = CreateCopyMat(DebMat[0], 0, 0, 3, 0);
    DebMat[2] = CreateCopyMat(DebMat[0], 1, 1, 1, 1);
    DebMat[3] = CreateCopyMat(DebMat[0], 1, 3, 1, 1);
    DebMat[4] = CreateCopyMat(DebMat[0], 0, 1, 1, 1);
    DebMat[5] = CreateCopyMat(DebMat[0], 0, 2, 1, 1);
    DebMat[6] = CreateCopyMat(DebMat[0], 0, 0, 0, 1);
    
    for (lp = 0; debtab[lp] != NULL; lp++) {
        if (debtab[lp]->rate == 0) {
            debtab[lp]->rate = 1;
        }
        debtab[lp]->uvanim = ((debtab[lp]->uw * 60) / debtab[lp]->rate);
        *(int*)&debtab[lp]->uvanim = (*(int*)&debtab[lp]->uvanim & 0xfffff800) + (debtab[lp]->uw * (debtab[lp]->frames - 1) & 0x3FF); //FIX
        debtab[lp]->alpslot[0].a = debtab[lp]->alpslot[0].a / debtab[lp]->etime;
        debtab[lp]->u1 = debtab[lp]->u1 + 524288.0f;
        debtab[lp]->u2 = debtab[lp]->u2 + 524288.0f;
        debtab[lp]->v1 = debtab[lp]->v1 + 524288.0f;
        debtab[lp]->v2 = debtab[lp]->v2 + 524288.0f;
    }
    
    buffer = (char*)((u32)(debbuffer + 0xf) & 0xfffffff0);
    
    freeDmaDebType = 0;
    
    size = 0x40800;
    for (lp = 0; lp < 256; lp++) {
        oldSize = size;
        freedebchunks[lp] = (struct rdata_s*)CreateDmaParticleSet(buffer, &oldSize);
        buffer += oldSize;
        size -= oldSize;
    }
    
    freedebchkptr = 0;
    for (lp = 0; lp < 0x100; lp++) {
        freedebkeys[lp] = lp;
    }
    
    freedebkeyptr = 0;
    for (lp = 0; lp < 256; lp++) {
        debkeydata[lp].active = 0;
        debkeydata[lp].pointer = 0;
        debkeydata[lp].rotory = 0;
        debkeydata[lp].rotorz = 0;
        debkeydata[lp].instances = 0;
        debkeydata[lp].type = 0;
    }
    
    for (lp = 0; lp < 0x20; lp++) {
        debris_emitter_stack[lp] = NULL;
    }
    
    debris_emitter_stack_index = 0;
    for (lp = 0; lp < 0x20; lp++) {
        debris_chunk_control_stack[lp] = NULL;
    }
    
    debris_chunk_control_stack_index = 0;
    for (lp = 0; lp < 0x200; lp++) {
        freechunkcontrols[lp] = &debris_chunk_controls[lp];
    }
    
    freechunkcontrolsptr = 0;
    for (lp = 0; lp < 128; lp++) {
        oldSize = size;
        DmaDebTypes[lp] = CreateDmaPartEffectList(buffer, &oldSize);
        buffer += oldSize;
        size -= oldSize;
    }
    
    for (lp = 0; lp < 0x100; lp++) {
        ParticleChunkRenderStack[lp].chunk = NULL;
        ParticleChunkRenderStack[lp].debinfo = NULL;
        ParticleChunkRenderStack[lp].debdata = NULL;
    }
    
    mydebbuffersize = (s32)buffer - (s32)debbuffer;
    return;
}

//NGC MATCH
s32 DebAlloc(void) {
  s32 key;
  
  if (freedebkeyptr > 0xff) {
   return -1;
  }
    key = (int)freedebkeys[freedebkeyptr];
    freedebkeyptr++;
    debkeydata[key].count = 0;
    debkeydata[key].debcount = 0;
    debkeydata[key].reqcount = 0;
    debkeydata[key].reqdebcount = 0;
    debkeydata[key].chunks[0] = NULL;
    return key;
}

//NGC MATCH
void DebReAlloc2(struct debkeydatatype_s *debkey) {
  s32 newchunksneeded;
  s32 i;
  s32 j;
  
    newchunksneeded = (s32)debkey->reqcount - (s32)debkey->count;
    if (newchunksneeded > 0) {
            if(newchunksneeded + freedebchkptr > 0x100) {
                return;
            }
          for (i = 0; i < newchunksneeded; i++) {
            debkey->chunks[i + debkey->count] = freedebchunks[i + freedebchkptr];
            for (j = 0; j < 0x20; j++) {
              (debkey->chunks[i + debkey->count]->debris[j].y) = 0.0f;
              (debkey->chunks[i + debkey->count]->debris[j].my) =  32768.0f;
            }
          }
        if (debkey->count == 0) {
          for (i = 0; i < 0x100; i++) {
            if (ParticleChunkRenderStack[i].chunk == NULL) {
               ParticleChunkRenderStack[i].chunk = (struct uv1deb*)*debkey->chunks;
               ParticleChunkRenderStack[i].debinfo = debtab[debkey->type];
               ParticleChunkRenderStack[i].debdata = debkey;
              break;
            }
          }
        }
        debkey->count = debkey->reqcount;
        debkey->debcount = debkey->reqdebcount;
        freedebchkptr = freedebchkptr + newchunksneeded;
        LinkDmaParticalSets((s32 **)debkey,(s32)debkey->reqcount);
      }

     else if ((freechunkcontrolsptr - newchunksneeded < 0x201)) {
            for (i = newchunksneeded; i < 0; i++) {
              freechunkcontrols[freechunkcontrolsptr]->chunk = (struct uv1deb*)debkey->chunks[i + debkey->count];
              freechunkcontrols[freechunkcontrolsptr]->delay = (s32)(debtab[debkey->type]->etime * 60.0f);
              freechunkcontrols[freechunkcontrolsptr]->action = DEBRIS_CHUNK_CONTROL_FREE;
              AddChunkControlToStack(freechunkcontrols[freechunkcontrolsptr],&debris_chunk_control_stack[debris_chunk_control_stack_index]);
              freechunkcontrolsptr++;
            }
          if (debkey->reqdebcount == 0) {
            for (i = 0; i < 0x100; i++) {
                if( ParticleChunkRenderStack[i].chunk == (struct uv1deb*)*debkey->chunks) {
                  ParticleChunkRenderStack[i].debinfo = debtab[debkey->type];
                  ParticleChunkRenderStack[i].debdata = NULL;
                  ParticleChunkRenderStack[i].rotmtx = debkey->rotmtx;
                  ParticleChunkRenderStack[i].x = debkey->x;
                  ParticleChunkRenderStack[i].y = debkey->y;
                  ParticleChunkRenderStack[i].z = debkey->z; 
                  break;
                }
              }
          } else {
            LinkDmaParticalSets((s32 **)&debkey->chunks[newchunksneeded + debkey->count],-newchunksneeded);
             for (i = 0; i < 0x100; i++) {
                if (ParticleChunkRenderStack[i].chunk == NULL) {
                    ParticleChunkRenderStack[i].chunk = (struct uv1deb*)debkey->chunks[newchunksneeded + debkey->count];
                    ParticleChunkRenderStack[i].debinfo = debtab[debkey->type];
                    ParticleChunkRenderStack[i].debdata = NULL;
                    ParticleChunkRenderStack[i].rotmtx = debkey->rotmtx;
                    ParticleChunkRenderStack[i].x = debkey->x;
                    ParticleChunkRenderStack[i].y = debkey->y;
                    ParticleChunkRenderStack[i].z = debkey->z;    
                    break;
                }
             }
          }
          debkey->debcount = debkey->reqdebcount;
          debkey->count = debkey->reqcount;
          if (debkey->reqdebcount == 0) {
            debkey->chunks[0] = NULL;
          }
          else {
            LinkDmaParticalSets((s32 **)debkey,(s32)debkey->reqcount);
          }
      }
  return;
}

//NGC MATCH
void DebReAlloc(struct debkeydatatype_s *debkey,s32 newdebcount) {
  s32 newchunksneeded;
  
  debkey->reqdebcount = (short)newdebcount;
  newchunksneeded = newdebcount + 0x1f;
  if (newchunksneeded < 0) {
    newchunksneeded = newdebcount + 0x3e;
  }
  debkey->reqcount = (newchunksneeded >> 5);
  if (debkey->reqcount == debkey->count) {
    debkey->debcount = (short)newdebcount;
  }
  else {
    DebReAlloc2(debkey);
  }
  return;
}

//NGC MATCH
void AddChunkControlToStack(struct debris_chunk_control_s *chunk,struct debris_chunk_control_s **stack) {
  struct debris_chunk_control_s *pdVar1;
    struct debris_chunk_control_s** current;
  
  for (current = stack; *current != NULL;) {
      pdVar1 = *current;
      stack = &pdVar1->next;
      current = &pdVar1->next;
  }
  *current = chunk;
  chunk->next = NULL;
  return;
}

//NGC MATCH
void RemoveChunkControlFromStack(struct debris_chunk_control_s *chunk,struct debris_chunk_control_s **stack) {
  struct debris_chunk_control_s *pdVar1;
  
  if (*stack != NULL) {
    if (*stack == chunk) {
      *stack = chunk->next;
    }
    else {
      do {
        pdVar1 = *stack;
        stack = &pdVar1->next;
        if (pdVar1->next == NULL) goto LAB_80080cb0;
      } while (pdVar1->next != chunk);
      pdVar1->next = chunk->next;
    }
  }
LAB_80080cb0:
  chunk->next = NULL;
  return;
}

//NGC MATCH
void DebFree(s32 *key) {
  s32 i;
  struct debkeydatatype_s **stack;
  
  if (*key != -1) {
        if (freechunkcontrolsptr + debkeydata[*key].count > 0x200) {
            DebFreeInstantly(key);
            return;
        }
        for(i = 0; i < debkeydata[*key].count; i++, freechunkcontrolsptr++) {
              freechunkcontrols[freechunkcontrolsptr]->chunk = (struct uv1deb *)debkeydata[*key].chunks[i];
              freechunkcontrols[freechunkcontrolsptr]->delay = (debtab[debkeydata[*key].type]->etime * 60.0f);
              freechunkcontrols[freechunkcontrolsptr]->action = DEBRIS_CHUNK_CONTROL_FREE;
              freechunkcontrols[freechunkcontrolsptr]->owner = &debkeydata[*key];
              stack = (struct debkeydatatype_s **)&debris_chunk_control_stack[debris_chunk_control_stack_index];
              AddChunkControlToStack(freechunkcontrols[freechunkcontrolsptr],(struct debris_chunk_control_s**)stack);
        }
          for (i = 0; i < 0x100; i++) {
              if (ParticleChunkRenderStack[i].chunk == debkeydata[*key].chunks[0]) {
                  ParticleChunkRenderStack[i].debdata = NULL;
                  ParticleChunkRenderStack[i].rotmtx = debkeydata[*key].rotmtx;
                  ParticleChunkRenderStack[i].x = debkeydata[*key].x;
                  ParticleChunkRenderStack[i].y = debkeydata[*key].y;
                  ParticleChunkRenderStack[i].z = debkeydata[*key].z;  
                  break;
              }
          }
          stack = FindDebrisEffectStack(&debkeydata[*key]);
          RemoveDebrisEffectFromStack(&debkeydata[*key],stack);
          debkeydata[*key].type = 0;
          freedebkeyptr--;
          freedebkeys[freedebkeyptr] = *key;
          *key = -1;
  }
  return;
}

//NGC MATCH
void DebFreeInstantly(s32 *key) {
  s32 i;
  s32 j;
  struct debkeydatatype_s **stack;
  
  if (*key != -1) {
    freedebchkptr = freedebchkptr - debkeydata[*key].count;
    for(i = 0; i < debkeydata[*key].count; i++) {
        freedebchunks[i + freedebchkptr] = debkeydata[*key].chunks[i];
        for(j = 0; j < 0x100; j++) {
          if (ParticleChunkRenderStack[j].chunk == debkeydata[*key].chunks[i]) {
            ParticleChunkRenderStack[j].chunk = NULL;
            ParticleChunkRenderStack[j].debinfo = NULL;
          }
        }
    }
    stack = FindDebrisEffectStack(&debkeydata[*key]);
    RemoveDebrisEffectFromStack(&debkeydata[*key],stack);
    debkeydata[*key].type = 0;
    freedebkeyptr--;
    freedebkeys[freedebkeyptr] = *key;
    *key = -1;
  }
  return;
}

//NGC MATCH
void DebFreeWithoutKey(struct debkeydatatype_s *debkeydatatofree) {
  s32 lp;
  s32 key;
  
  for(lp = 0; lp <= 0xff; lp++) {
        if (&debkeydata[lp] == debkeydatatofree) {
              key = lp;
              DebFree(&key);
              break;
        }
  }
  return;
}

//NGC MATCH
void DebrisEmiterPos(s32 key,float x,float y,float z) {
  if (key == -1) {
    return;
  }
  debkeydata[key].x = x;
  debkeydata[key].y = y;
  debkeydata[key].z = z;
  return;
}

//NGC MATCH
void DebrisStartOffset(s32 key,s32 offset) {
  struct debinftype *debinfo;
  s32 looptime;
  s32 loopfrac;
  
  if (key != -1) {
      debinfo = debtab[debkeydata[key].type];
      if ((debinfo->ival_on_ran != 0) || (debinfo->ival_off_ran != 0)) {
          debkeydata[key].delay = 0;
      }
      else {
          if (debinfo->gensort == 7) {
              debkeydata[key].delay = 0;
              debkeydata[key].rotory = (short)(offset * (debinfo->variable_emit).y);
              debkeydata[key].rotorz = (short)(offset * (debinfo->variable_emit).z);
          }
          looptime = debinfo->ival_on + debinfo->ival_off;
          loopfrac = (globalframes % looptime);
          loopfrac -= offset;
          debkeydata[key].delay = (loopfrac != 0) ? looptime - loopfrac : loopfrac; 
          debkeydata[key].oncount = debinfo->ival_on;
      }
  }
  return;
}

//NGC MATCH
void DebrisOrientation(s32 key,short rotz,short roty) {
  if (key != -1) {
    debkeydata[key].rotmtx = numtx_identity;
    NuMtxRotateZ(&debkeydata[key].rotmtx,rotz);
    NuMtxRotateY(&debkeydata[key].rotmtx,roty);
  }
  return;
}

//NGC MATCH
void DebrisEmitterOrientation(s32 key,short emitrotz,short emitroty) {
  if (key != -1) {
    debkeydata[key].emitrotmtx = numtx_identity;
    NuMtxRotateZ(&debkeydata[key].emitrotmtx,emitrotz);
    NuMtxRotateY(&debkeydata[key].emitrotmtx,emitroty);
  }
  return;
}

//NGC MATCH
void DebrisEmitterOrientationMtx(s32 key,struct numtx_s *emitrotmtx) {
  if (key == -1) {
    return;
  }
  debkeydata[key].emitrotmtx = *emitrotmtx;
  debkeydata[key].emitrotmtx._30 = 0.0f;
  debkeydata[key].emitrotmtx._31 = 0.0f;
  debkeydata[key].emitrotmtx._32 = 0.0f;
  return;
}

//NGC MATCH
void DebrisReflectionOrientation(s32 key,short refrotz,short refroty,float refoff,float refbounce) {
  if (key == -1) {
    return;
  }
  debkeydata[key].refrotz = refrotz;
  debkeydata[key].refroty = refroty;
  debkeydata[key].refoff = refoff;
  debkeydata[key].refbounce = refbounce;
  return;
}

//NGC MATCH
void DebrisSetTrigger(s32 key,s32 trigger_type,s32 trigger_id,float trigger_var) {
  if (key == -1) {
    return;
  }
  debkeydata[key].trigger_type = trigger_type;
  debkeydata[key].trigger_id = trigger_id;
  debkeydata[key].trigger_var = trigger_var;
  return;
}

//NGC MATCH
void DebrisSetGroupID(s32 key,short group_id) {
  if (key == -1) {
    return;
  }
  debkeydata[key].group_id = group_id;
  return;
}

//NGC MATCH
struct debkeydatatype_s ** FindDebrisEffectStack(struct debkeydatatype_s *effect) {
  struct debkeydatatype_s **current;
  struct debkeydatatype_s *pdVar4;
  s32 lp;

  for (lp = 0; lp < 0x20; lp++) {
        current = &debris_emitter_stack[lp];
        if (debris_emitter_stack[lp] != 0) {
          do {
            pdVar4 = *current;
            if (pdVar4 == effect) {
              return &debris_emitter_stack[lp];
            }
            current = &pdVar4->next;
          } while (pdVar4->next != NULL);
        }
  }
  return NULL;
}

//NGC MATCH
void AddDebrisEffectToStack(struct debkeydatatype_s *effect,struct debkeydatatype_s **stack) {
  struct debkeydatatype_s *rdat;
  
  if (effect->next != NULL) {
    effect->next = NULL;
  }
  while (*stack != NULL) {
    rdat = *stack;
    stack = &rdat->next;
  }
  *stack = effect;
  return;
}

//NGC MATCH
void RemoveDebrisEffectFromStack(struct debkeydatatype_s *effect,struct debkeydatatype_s **stack) {
  struct debkeydatatype_s *current;
  
  if (stack == NULL) {
    return;
  }
  if (effect == NULL) {
    return;
  }
  if (*stack != NULL) {
    if (*stack == effect) {
      *stack = effect->next;
    }
    else {
        do {
            current = *stack;
            stack = &current->next;
            if (current->next == NULL) goto LAB_80081630;
        } while (current->next != effect);
        current->next = effect->next;
    }
  }
LAB_80081630:
  effect->next = NULL;
  return;
}

//NGC MATCH
void AddFiniteShotDebrisEffect(s32 *key,s32 type,struct nuvec_s *pos,s32 repeats) {
  AddDebrisEffect(key,type,pos->x,pos->y,pos->z);
  if (*key != -1) {
    debkeydata[*key].instances = repeats;
    debkeydata[*key].delay = 0;
  }
  return;
}

//NGC MATCH
void AddVariableShotDebrisEffectMtx2(s32 type,struct nuvec_s *pos,s32 numdeb,struct numtx_s *emitrotmtx,struct numtx_s *rotmtx) {
    
    struct debinftype* debinfo; // r27
    struct uv1deb* deb; // 
    s32 i; // r31
    s32 j; // r8
    s32 chunks_needed; // r30
    s32 new_total_chunks; // r28
    float camdist; // f1
    struct debris_chunk_control_s* checkingnow; // r10
    struct debris_chunk_control_s* controlfound; // r26
    s32 controlfoundindex; // r23
    struct debkeydatatype_s* dkey; // r29

    //temp
    s32 iVar3;
    s32 iVar7;
    s32 iVar15;
    //

    controlfound = 0;
    chunks_needed = 0;
    if (type != -1) {
        debinfo = debtab[type];
        camdist = CameraEmitterDistance(pos);
          
        if ((camdist < debinfo->cuton) || (camdist >= debinfo->cutoff)) {
            return;
        }
          
        if (debinfo->variable_key == -1) {
            if (0xff < freedebkeyptr) {
                return;
            }
            debinfo->variable_key = freedebkeys[freedebkeyptr];
            freedebkeyptr++;
            dkey = &debkeydata[debinfo->variable_key];
            dkey->count = 0;
            dkey->debcount = 0;
            dkey->pointer = 0;
            dkey->genptr = (struct uv1deb*(*)())gensorttab[debinfo->gensort];
            dkey->gencode = gencodetab[debinfo->gencode];
            dkey->rotmtx = *rotmtx;
            dkey->rotmtx._30 = 0.0f;
            dkey->rotmtx._31 = 0.0f;
            dkey->rotmtx._32 = 0.0f;
            DebrisEmiterPos(debinfo->variable_key, 0.0f, 0.0f, 0.0f);
        } else {
            dkey = &debkeydata[debinfo->variable_key];
        }
        
        if (dkey->pointer + numdeb > dkey->count << 5) {
            if (dkey->count < 0) {
                dkey->count = 0;
            }
            iVar15 = dkey->pointer + numdeb;
            iVar7 = iVar15 + 0x1f;
            if (iVar7 < 0) {
                iVar7 = iVar15 + 0x3e;
            }
            iVar3 = iVar7 >> 5;
            new_total_chunks = iVar3 - dkey->count;
            if (0xff < freedebchkptr + new_total_chunks) {
                return;
            }
            for (i = 0; i < new_total_chunks; i++) {
                dkey->chunks[dkey->count++] = freedebchunks[freedebchkptr++];
            }
            dkey->count = (short)iVar3;
            LinkDmaParticalSets((s32 **)dkey, iVar3);
            if (iVar3 == new_total_chunks) {
                for (i = 0; i < 0x100; i++) {
                    if (ParticleChunkRenderStack[i].chunk == NULL) {
                        ParticleChunkRenderStack[i].chunk = (struct uv1deb*)dkey->chunks[0];
                        ParticleChunkRenderStack[i].debinfo = debinfo;
                        ParticleChunkRenderStack[i].debdata = dkey;
                        break;
                    }
                }
            }
        }
        
        dkey->debcount += numdeb;
        dkey->toffx = pos->x;
        dkey->toffy = pos->y;
        dkey->toffz = pos->z;
        DebrisEmitterOrientationMtx(debinfo->variable_key, emitrotmtx);
        
        for (i = 0; i < numdeb; i++) {
            (dkey->genptr)(dkey, debinfo);
        }
    if(dkey->disposed < dkey->count) {
        for (i = dkey->disposed; i < dkey->count; i++) {
            if (freechunkcontrolsptr < 0x200) {
                freechunkcontrols[freechunkcontrolsptr]->chunk = (struct uv1deb*)dkey->chunks[i];
                freechunkcontrols[freechunkcontrolsptr]->delay = debinfo->etime * 60.0f;
                freechunkcontrols[freechunkcontrolsptr]->action = DEBRIS_CHUNK_CONTROL_FREE_AND_UNLINK;
                freechunkcontrols[freechunkcontrolsptr]->owner = dkey;
                AddChunkControlToStack(freechunkcontrols[freechunkcontrolsptr], &debris_chunk_control_stack[debris_chunk_control_stack_index]);
                freechunkcontrolsptr++;
                dkey->disposed++;
            }
        }
    }
        if (dkey->disposed == dkey->count) {
            checkingnow = debris_chunk_control_stack[j];
            for (j = 0; j < 0x20; j++) {
                for (checkingnow = debris_chunk_control_stack[j]; checkingnow != NULL; checkingnow = checkingnow->next) {
                    if (checkingnow->chunk == (struct uv1deb*)dkey->chunks[dkey->disposed - 1]) {
                      controlfound = checkingnow;
                      chunks_needed = j;
                      break;
                    }
                }
                if(controlfound != NULL) {
                    break;
                }
            }
            if (controlfound != NULL) {
                controlfound->delay = (s32)(debinfo->etime * 60.0f);
                if (chunks_needed != debris_chunk_control_stack_index) {
                    RemoveChunkControlFromStack(controlfound, &debris_chunk_control_stack[chunks_needed]);
                    AddChunkControlToStack(controlfound, &debris_chunk_control_stack[debris_chunk_control_stack_index]);
                }
            }
        }
    }
    return;
}

//NGC MATCH
void AddVariableShotDebrisEffectMtx(s32 type,struct nuvec_s *pos,s32 numdeb,short emitrotz,short emitroty,struct numtx_s *rotmtx) {
  struct numtx_s tmtx;

  tmtx = numtx_identity;
  NuMtxRotateZ(&tmtx,emitrotz);
  NuMtxRotateY(&tmtx,emitroty);
  AddVariableShotDebrisEffectMtx2(type,pos,numdeb,&tmtx,rotmtx);
  return;
}

//NGC MATCH
void AddVariableShotDebrisEffect(s32 type,struct nuvec_s *pos,s32 numdeb,short emitrotz,short emitroty) {
  AddVariableShotDebrisEffectMtx(type,pos,numdeb,emitrotz,emitroty,&numtx_identity);
  return;
}

//NGC MATCH
void AddDebrisEffect(s32 *key,s32 type,float x,float y,float z) {
  s32 i;
  struct debinftype *debinfo;

  if (type != -1) {
    if (*key == -1) {
      *key = DebAlloc();
      if (*key == -1) {
        return;
      }
    }
    debinfo = debtab[type];
    debkeydata[*key].type = (short)type;
    debkeydata[*key].active = 1;
    DebrisStartOffset(*key,(s32)debinfo->ival_offset);
    debkeydata[*key].oncount = debtab[type]->ival_on + (s32)((randy() * debtab[type]->ival_on_ran) * 0.125); //4.656612873077393e-10 --> asm (lfd)
    debkeydata[*key].genptr = gensorttab[debinfo->gensort];
    debkeydata[*key].gencode = gencodetab[debinfo->gencode];
    debkeydata[*key].rotory = 0;
    debkeydata[*key].rotorz = 0;
    debkeydata[*key].sphere_next = 0;
    debkeydata[*key].sphere_next_emit = 1;
    for (i = 0; i < debtab[type]->numspheres; i++) {
        debkeydata[*key].spheres[i].t = 0xbf800000;
    }
    debkeydata[*key].toffx = 0.0f;
    debkeydata[*key].toffy = 0.0f;
    debkeydata[*key].toffz = 0.0f;
    DebrisEmiterPos(*key,x,y,z);
    DebrisEmitterOrientation(*key,0,0);
    DebrisOrientation(*key,0,0);
    DebrisReflectionOrientation(*key,0,0,0.0f,0.9f);
    AddDebrisEffectToStack(debkeydata + *key,debris_emitter_stack + debris_emitter_stack_index);
  }
  return;
}

void DebrisSetup(void) {
  SetupDebris();
  return;
}

void DebrisSetRenderGroup(s32 group) {
  debris_render_group = group;
  return;
}

//NGC MATCH
void DebrisDoSounds(struct debkeydatatype_s *debkey,s32 flag) {
  struct debinftype *dt;
  s32 i;
  
  dt = debtab[debkey->type];
  if (dt != NULL) {
    for(i = 0; i < 4; i++) {
          if ((dt->sounds[i].id != -1) && (dt->sounds[i].type == flag)) {
                if (debkey->sound_next[i] < 1) {
                      debris_sfx = 1;
                      edbitsSoundPlay(&debkey->x,dt->sounds[i].id);
                      if (flag == 1 || flag == 2) {
                        debkey->sound_next[i] = 9999;
                      }
                      else {
                        debkey->sound_next[i] = dt->sounds[i].delay;
                      }
                }
          }
    }
  }
  return;
}

//NGC MATCH
s32 LookupDebrisEffect(char *name) {
  s32 i;
  
  for (i = 1; i < 128; i++) {
    if ((debtab[i] != NULL) && (strcmp((debtab[i])->id,name) == 0)) return i;
  }
    return -1;
}

struct nuvec_s* CutoffCameraVec;

//NGC MATCH
void DebrisRegisterCutoffCameraVec(struct nuvec_s *cutoff) {
  CutoffCameraVec = cutoff;
  return;
}

//NGC MATCH
float CameraEmitterDistance(struct nuvec_s *vec) {
  if (CutoffCameraVec == NULL) {
    return 0.0f;
  }
  else {
    return NuVecDist(vec,CutoffCameraVec,NULL);
  }
}

//NGC MATCH
s32 DebrisCollisionCheck(struct nuvec_s *centre, float radius) {   
    s32 i;
    s32 j;
    s32 k;
    struct debinftype *dt;
    float ttime;
    float dtime;
    float tt;
    float tr;
    float stime;
    struct nuvec_s tvec;
  
    tr = 0.0f;
    for (i = 0; i < 0x100; i++) {
        if ((debkeydata[i].type == -1) || (debkeydata[i].type == 0)) {
            continue;
        }
        dt = debtab[debkeydata[i].type];
        if (dt->numspheres == 0) {
            continue;
        }
        for (j = 0; j < dt->numspheres; j++) {
            stime = debkeydata[i].spheres[j].t;
            if (stime != -1.0f) {
                tvec.x = debkeydata[i].spheres[j].emit.x * stime;
                tvec.y = debkeydata[i].spheres[j].emit.y * stime;
                tvec.z = debkeydata[i].spheres[j].emit.z * stime;
                
                tvec.y = (dt->grav * (stime * stime)) + tvec.y;
                
                tvec.x += debkeydata[i].x;
                tvec.y += debkeydata[i].y;
                tvec.z += debkeydata[i].z;              
                
                tt = stime / dt->etime;
                for (k = 0; k < 7; k++) {
                    if ((dt->sphereslot[k].t <= tt) && (dt->sphereslot[k + 1].t >= tt)) {
                        break;
                    }
                }
                tr = (((tt - dt->sphereslot[k].t) / (dt->sphereslot[k + 1].t - dt->sphereslot[k].t ))
                * (dt->sphereslot[k + 1].r - dt->sphereslot[k].r) + dt->sphereslot[k].r);
            }
            if (NuVecDistSqr(centre, &tvec, NULL) < (radius + tr) * (radius + tr)) {
                return i;
            }
        }
    }
    return -1;
}