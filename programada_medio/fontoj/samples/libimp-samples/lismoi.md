


# sample-Ai : fonctionne
  génère un fichier ai_record.pcm, à lire avec :

```
ffplay -f s16le -ar 8k -ac 1 ai_record.pcm
```

# sample-Ai-Ref :
*  génère ref_test_record.pcm, ref_test_for_play.pcm et ref_test_ref.pcm
*  ref_test_ref.pcm est un enregistrement de 40s. OK.
   *  ffplay -f s16le -ar 8k -ac 1 ref_test_ref.pcm

# sample-Ai-AEC :
* génère un fichier
