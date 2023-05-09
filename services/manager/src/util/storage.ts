import { observer } from '@finwo/observer';

type StorageValue = null | number | string | StorageEntry;
type StorageEntry = {
  [index:string]: StorageValue;
} | StorageValue[];

type CacheEntry = {
  q: (()=>void)[],
  data: any,
};

const cache: Record<string, CacheEntry> = {};

export async function syncedObject<T extends StorageEntry>(
  name     : string,
  callback : ()=>void = undefined
): Promise<T> {
  // Return from cache if possible, so it's the exact same object everywhere
  if (cache[name]) {
    if (callback) cache[name].q.push(callback);
    return cache[name].data;
  }

  // Bind cache object to actual storage
  // @ts-ignore storage_get is set by webview wrapper
  const org: T = await storage_get(name) || {};
  const obs    = observer<T>(org, async () => {
    // @ts-ignore storage_set is set by the webview wrapper
    storage_set(name, org);
    for (const fn of cache[name].q) await fn();
  });

  // Return the newly constructed bound object
  cache[name] = {
    q    : [callback].filter(f => f),
    data : obs
  };

  return cache[name].data;
}
