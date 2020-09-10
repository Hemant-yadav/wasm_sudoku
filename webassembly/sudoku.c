#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define CELL_LENGTH 3
#define GROUP_DEPTH 2
#define LENGTH (CELL_LENGTH * CELL_LENGTH)
#define TOTAL (LENGTH * LENGTH)
#define MASK ((2 << LENGTH) - 1)

#define fc for (char c = 0; c < LENGTH; c++)
#define fd for (char d = 0; d < LENGTH; d++)
#define fi for (char i = 0; i < LENGTH; i++)
#define fj for (char j = 0; j < LENGTH; j++)
#define fk for (char k = 0; k < LENGTH; k++)
#define ft for (char t = 0; t < TOTAL; t++)
#define fij                         \
  for (char i = 0; i < LENGTH; i++) \
    for (char j = 0; j < LENGTH; j++)

short countBits(short n)
{
  short count = 0;
  while (n)
    n >>= (++count) && 1;
  return count;
}
void filterCell(char *sudoku, short *map, char i, char j)
{
  char e = sudoku[i * LENGTH + j];
  fk
  {
    // Filter column
    map[k * LENGTH + j] &= ~(1 << e);
    // Filter row
    map[i * LENGTH + k] &= ~(1 << e);
    // Filter cell grid
    map[(i / CELL_LENGTH * CELL_LENGTH + k / CELL_LENGTH) * LENGTH + j / CELL_LENGTH * CELL_LENGTH + k % CELL_LENGTH] &= ~(1 << e);
  }
  // Mark as filtered
  map[i * LENGTH + j] = 1;
}
bool filter(char *sudoku, short *map)
{
  bool updated = false;
  fij
  {
    if ((map[i * LENGTH + j] & 1) && (map[i * LENGTH + j] != 1))
    {
      char e = sudoku[i * LENGTH + j];
      updated = true;
      filterCell(sudoku, map, i, j);
    }
  }
  return updated;
}

bool fillCell(char *sudoku, short *map, short *meta, char pos)
{
  char count;
  char v;
  if (map[pos] != 1)
  {
    count = 0;
    fk
    {
      if (map[pos] & (2 << k))
      {
        v = k + 1;
        count++;
      }
      if (count > 1)
        break;
    }
    if (count == 1)
    {
      sudoku[pos] = v;
      map[pos] |= 1;
      filterCell(sudoku, map, pos / LENGTH, pos % LENGTH);
      return true;
    }
  }
  return false;
}
char fillRow(char *sudoku, short *map, short *meta, char row)
{
  char count;
  char updated = 0;
  char v;
  fk if (!(meta[row] & (2 << k)))
  {
    count = 0;
    fc
    {
      if (map[row * LENGTH + c] & (2 << k))
      {
        v = row * LENGTH + c;
        count++;
      }
      if (count > 1)
        break;
    }
    if (count == 1)
    {
      updated++;
      sudoku[v] = k + 1;

      map[v] |= 1;
      meta[row] |= (2 << k);
      filterCell(sudoku, map, row, v % LENGTH);
    }
  }
  return updated;
}
char fillColumn(char *sudoku, short *map, short *meta, char column)
{
  char count;
  char updated = 0;
  char v;
  fk if (!(meta[LENGTH + column] & (2 << k)))
  {
    count = 0;
    fc
    {
      if (map[c * LENGTH + column] & (2 << k))
      {
        v = c * LENGTH + column;
        count++;
      }
      if (count > 1)
        break;
    }
    if (count == 1)
    {
      updated++;
      sudoku[v] = k + 1;

      map[v] |= 1;
      meta[LENGTH + column] |= (2 << k);
      filterCell(sudoku, map, v / LENGTH, column);
    }
  }
  return updated;
}
char fillGrid(char *sudoku, short *map, short *meta, char cursor)
{
  char count;
  char updated = 0;
  char v;
  fk if (!(meta[LENGTH + LENGTH + cursor] & (2 << k)))
  {
    count = 0;
    fc
    {
      if (map[cursor / CELL_LENGTH * CELL_LENGTH * LENGTH +
              cursor % CELL_LENGTH * CELL_LENGTH +
              c / CELL_LENGTH * LENGTH +
              c % CELL_LENGTH] &
          (2 << k))
      {
        v = cursor / CELL_LENGTH * CELL_LENGTH * LENGTH +
            cursor % CELL_LENGTH * CELL_LENGTH +
            c / CELL_LENGTH * LENGTH +
            c % CELL_LENGTH;
        count++;
      }
      if (count > 1)
        break;
    }
    if (count == 1)
    {
      updated++;
      sudoku[v] = k + 1;

      map[v] |= 1;
      meta[LENGTH + LENGTH + cursor] |= (2 << k);
      filterCell(sudoku, map, v / LENGTH, v % LENGTH);
    }
  }
  return updated;
}

char fillCells(char *sudoku, short *map, short *meta)
{
  char updated = 0;
  ft updated += fillCell(sudoku, map, meta, t);
  return updated;
}
char fillRows(char *sudoku, short *map, short *meta)
{
  char updated = 0;
  fd updated += fillRow(sudoku, map, meta, d);
  return updated;
}
char fillColumns(char *sudoku, short *map, short *meta)
{
  char updated = 0;
  fd updated += fillColumn(sudoku, map, meta, d);
  return updated;
}
char fillGrids(char *sudoku, short *map, short *meta)
{
  char updated = 0;
  fd updated += fillGrid(sudoku, map, meta, d);
  return updated;
}

char fill(char *sudoku, short *map, short *meta)
{
  return fillCells(sudoku, map, meta) + fillRows(sudoku, map, meta) + fillColumns(sudoku, map, meta) + fillGrids(sudoku, map, meta);
}

char gridRow(char *sudoku, short *map, short *meta)
{
  char updated = 0;
  fk fd
  {
    if (!(meta[d] & (2 << k)))
    {
      short n = -1;
      fc
      {
        if (map[d * LENGTH + c] & (2 << k))
        {
          if (n == -1)
            n = c / CELL_LENGTH;
          else if (n != c / CELL_LENGTH)
          {
            n = -2;
            break;
          }
        }
      }
      if (n >= 0)
      {
        fc
        {
          if (c / CELL_LENGTH == (d % CELL_LENGTH))
          {
            continue;
          }
          map[d / CELL_LENGTH * CELL_LENGTH * LENGTH +
              n * CELL_LENGTH +
              c / CELL_LENGTH * LENGTH +
              c % CELL_LENGTH] &= ~(2 << k);
        }
        updated += fillGrid(sudoku, map, meta, d / CELL_LENGTH * CELL_LENGTH + n);
      }
    }
  }
  return updated;
}
char gridColumn(char *sudoku, short *map, short *meta)
{
  char updated = 0;
  fk fd
  {
    if (!(meta[d + LENGTH] & (2 << k)))
    {
      short m = -1;
      fc
      {
        if (map[c * LENGTH + d] & (2 << k))
        {
          if (m == -1)
            m = c / CELL_LENGTH;
          else if (m != c / CELL_LENGTH)
          {
            m = -2;
            break;
          }
        }
      }
      if (m >= 0)
      {
        fc
        {
          if (c % CELL_LENGTH == (d % CELL_LENGTH))
          {
            continue;
          }
          map[m * CELL_LENGTH * LENGTH +
              d / CELL_LENGTH * CELL_LENGTH +
              c / CELL_LENGTH * LENGTH +
              c % CELL_LENGTH] &= ~(2 << k);
        }
        updated += fillGrid(sudoku, map, meta, m * CELL_LENGTH + d % CELL_LENGTH);
      }
    }
  }
  return updated;
}
char gridGrid(char *sudoku, short *map, short *meta)
{
  char updated = 0;
  fk fd if (!(meta[d + LENGTH + LENGTH] & (2 << k)))
  {
    short m = -1;
    short n = -1;
    fc
    {
      if (map[d / CELL_LENGTH * CELL_LENGTH * LENGTH +
              d % CELL_LENGTH * CELL_LENGTH +
              c / CELL_LENGTH * LENGTH +
              c % CELL_LENGTH] &
          (2 << k))
      {
        if (m == -1)
          m = c / CELL_LENGTH;
        else if (m != c / CELL_LENGTH)
          m = -2;

        if (n == -1)
          n = c % CELL_LENGTH;
        else if (n != (c % CELL_LENGTH))
          n = -2;

        if (m == -2 && n == -2)
          break;
      }
    }
    if (m >= 0)
    {
      fc
      {
        if (c / CELL_LENGTH == d % CELL_LENGTH)
        {
          continue;
        }
        map[d / CELL_LENGTH * CELL_LENGTH * LENGTH + m * LENGTH + c] &= ~(2 << k);
      }
      updated += fillRow(sudoku, map, meta, d / CELL_LENGTH * CELL_LENGTH + m);
    }
    if (n >= 0)
    {
      fc
      {
        if ((c / CELL_LENGTH) == (d / CELL_LENGTH))
        {
          continue;
        }
        map[c * LENGTH + d % CELL_LENGTH * CELL_LENGTH + n] &= ~(2 << k);
      }
      updated += fillColumn(sudoku, map, meta, d % CELL_LENGTH * CELL_LENGTH + n);
    }
  }
  return updated;
}
char gridline(char *sudoku, short *map, short *meta)
{
  return gridRow(sudoku, map, meta) + gridColumn(sudoku, map, meta) + gridGrid(sudoku, map, meta);
}

char multiRow(char *sudoku, short *map, short *meta)
{
  char updated = 0;
  fd if ((meta[d] & (MASK | 1)) != MASK)
  {
    short histo[LENGTH] = {0};
    fc
    {
      if (map[d * LENGTH + c] & 1)
        continue;
      short number = map[d * LENGTH + c] >> 1;
      fi
      {
        if (number & 1)
          histo[i]++;
        number >>= 1;
        if (number == 0)
          break;
      }
    }
    // All groups from 2 to LENGTH - 1
    for (char c = 2; c <= GROUP_DEPTH; c++)
    {
      // All values from 0 (1 << 0 => 1) to LENGTH
      fi
      {
        if (histo[i] != c)
          continue;
        short mux = MASK;
        char runs = 0;
        fj
        {
          if (map[d * LENGTH + j] & (2 << i))
          {
            mux &= map[d * LENGTH + j];
            runs++;
          }
        }
        if (runs != c)
          continue;
        if (countBits(mux >> 1) > c)
        {
          char member_count = 1;
          short fix = 2 << i;
          for (char j = i + 1; j < LENGTH; j++)
          {
            if (histo[j] != c)
              continue;
            if (mux & (2 << j))
            {
              member_count++;
              fix |= 2 << j;
              if (member_count == c)
                break;
            }
          }
          if (member_count == c)
          {
            fj
            {
              if (map[d * LENGTH + j] & (2 << i))
              {
                map[d * LENGTH + j] &= fix;
                meta[3 * LENGTH + d] |= 1 << j;
              }
              else if (!(map[d * LENGTH + j] & 1))
              {
                map[d * LENGTH + j] &= ~(fix);
              }
            }
            updated += fillRow(sudoku, map, meta, d);
          }
        }
      }
    }
  }
  return updated;
}
char multiColumn(char *sudoku, short *map, short *meta)
{
  char updated = 0;
  fd if ((meta[LENGTH + d] & (MASK | 1)) != MASK)
  {
    short histo[LENGTH] = {0};
    fc
    {
      if (map[c * LENGTH + d] & 1)
        continue;
      short number = map[c * LENGTH + d] >> 1;
      fi
      {
        if (number & 1)
          histo[i]++;
        number >>= 1;
        if (number == 0)
          break;
      }
    }
    // All groups from 2 to LENGTH - 1
    for (char c = 2; c <= GROUP_DEPTH; c++)
    {
      // All values from 0 (1 << 0 => 1) to LENGTH
      fi
      {
        if (histo[i] != c)
          continue;
        short mux = MASK;
        char runs = 0;
        fj
        {
          if (map[j * LENGTH + d] & (2 << i))
          {
            mux &= map[j * LENGTH + d];
            runs++;
          }
        }
        if (runs != c)
          continue;
        if (countBits(mux >> 1) > c)
        {
          char member_count = 1;
          short fix = 2 << i;
          for (char j = i + 1; j < LENGTH; j++)
          {
            if (histo[j] != c)
              continue;
            if (mux & (2 << j))
            {
              member_count++;
              fix |= 2 << j;
              if (member_count == c)
                break;
            }
          }
          if (member_count == c)
          {
            fj
            {
              if (map[j * LENGTH + d] & (2 << i))
              {
                map[j * LENGTH + d] &= fix;
                meta[3 * LENGTH + j] |= 1 << d;
              }
              else if (!(map[j * LENGTH + d] & 1))
              {
                map[j * LENGTH + d] &= ~(fix);
              }
            }
            updated += fillColumn(sudoku, map, meta, d);
          }
        }
      }
    }
  }
  return updated;
}
char multiGrid(char *sudoku, short *map, short *meta)
{
  char updated = 0;
  fd if ((meta[LENGTH + LENGTH + d] & (MASK | 1)) != MASK)
  {
    short histo[LENGTH] = {0};
    fc
    {
      if (map[d / CELL_LENGTH * CELL_LENGTH * LENGTH +
              d % CELL_LENGTH * CELL_LENGTH +
              c / CELL_LENGTH * LENGTH +
              c % CELL_LENGTH] &
          1)
        continue;
      short number = map[d / CELL_LENGTH * CELL_LENGTH * LENGTH +
                         d % CELL_LENGTH * CELL_LENGTH +
                         c / CELL_LENGTH * LENGTH +
                         c % CELL_LENGTH] >>
                     1;
      fi
      {
        if (number & 1)
          histo[i]++;
        number >>= 1;
        if (number == 0)
          break;
      }
    }
    // All groups from 2 to LENGTH - 1
    for (char c = 2; c < GROUP_DEPTH; c++)
    {
      // All values from 0 (1 << 0 => 1) to LENGTH
      fi
      {
        if (histo[i] != c)
          continue;
        short mux = MASK;
        char runs = 0;
        fj
        {
          if (map[d / CELL_LENGTH * CELL_LENGTH * LENGTH +
                  d % CELL_LENGTH * CELL_LENGTH +
                  j / CELL_LENGTH * LENGTH +
                  j % CELL_LENGTH] &
              (2 << i))
          {
            mux &= map[d / CELL_LENGTH * CELL_LENGTH * LENGTH +
                       d % CELL_LENGTH * CELL_LENGTH +
                       j / CELL_LENGTH * LENGTH +
                       j % CELL_LENGTH];
            runs++;
          }
        }
        if (runs != c)
          continue;
        if (countBits(mux >> 1) > c)
        {
          char member_count = 1;
          short fix = 2 << i;
          for (char j = i + 1; j < LENGTH; j++)
          {
            if (histo[j] != c)
              continue;
            if (mux & (2 << j))
            {
              member_count++;
              fix |= 2 << j;
              if (member_count == c)
                break;
            }
          }
          if (member_count == c)
          {
            fj
            {
              if (map[d / CELL_LENGTH * CELL_LENGTH * LENGTH +
                      d % CELL_LENGTH * CELL_LENGTH +
                      j / CELL_LENGTH * LENGTH +
                      j % CELL_LENGTH] &
                  (2 << i))
              {
                meta[3 * LENGTH + d / CELL_LENGTH * CELL_LENGTH + j / CELL_LENGTH] |= 1 << (d % CELL_LENGTH * CELL_LENGTH + j % CELL_LENGTH);
                map[d / CELL_LENGTH * CELL_LENGTH * LENGTH +
                    d % CELL_LENGTH * CELL_LENGTH +
                    j / CELL_LENGTH * LENGTH +
                    j % CELL_LENGTH] &= fix;
              }
              else
              {
                map[d / CELL_LENGTH * CELL_LENGTH * LENGTH +
                    d % CELL_LENGTH * CELL_LENGTH +
                    j / CELL_LENGTH * LENGTH +
                    j % CELL_LENGTH] &= ~(fix);
              }
            }
            updated += fillGrid(sudoku, map, meta, d / CELL_LENGTH * CELL_LENGTH + d % CELL_LENGTH);
          }
        }
      }
    }
  }
  return updated;
}
char multiline(char *sudoku, short *map, short *meta)
{
  return multiRow(sudoku, map, meta) + multiColumn(sudoku, map, meta) + multiGrid(sudoku, map, meta);
}

char solver(char *sudoku, short *map, short *meta)
{
  memset(meta, 0, 4 * LENGTH * sizeof(*meta));
  char remaining = TOTAL;
  ft
      map[t] = (MASK ^ 1) | (sudoku[t] && remaining--);
  short v;
  for (char loopO = 1; remaining > 0 && loopO > 0; loopO--)
  {
    v = 0;
    filter(sudoku, map);
    v += multiline(sudoku, map, meta);
    v += gridline(sudoku, map, meta);
    v += fill(sudoku, map, meta);
    if (v)
    {
      remaining -= v;
      loopO = 2;
    }
  }
  if (remaining)
  {
    char min = TOTAL;
    char pos = -1;
    char copysudoku[TOTAL];
    short copymap[TOTAL];
    short copymeta[4 * LENGTH];
    ft if (meta[3 * LENGTH + t / LENGTH] & (1 << (t % LENGTH)))
    {
      if (!sudoku[t] && !(map[t] & 1) && (countBits(map[t]) < min))
      {
        min = countBits(map[t]);
        pos = t;
      }
    }
    if (pos == -1)
    {
      ft if (!sudoku[t] && !(map[t] & 1) && (countBits(map[t]) < min))
      {
        min = countBits(map[t]);
        pos = t;
      }
    }
    if (pos >= 0)
    {
      fk if (map[pos] & (2 << k))
      {
        ft copysudoku[t] = sudoku[t];
        copysudoku[pos] = k + 1;
        if (solver(copysudoku, copymap, copymeta) == 0)
        {
          ft sudoku[t] = copysudoku[t];
          remaining = 0;
          break;
        }
      }
    }
  }
  return remaining;
}
